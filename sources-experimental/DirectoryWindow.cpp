
#include "DirectoryWindow.h"
#include "Utils.h"
#include <Entry.h>
#include <Message.h>
#include "BPLocale.h"
#include <StorageKit.h>

#include "DirectoryItem.h"
#include <ScrollView.h>
#include <Button.h>
#include <View.h>

#include "Logger.h"
#include "Utils.h"

static BBitmap* fItemIcon;
static BBitmap* fFolderIcon; //?
extern BMessage Directory_list; //?


#include "MainWindow.h"
extern MainWindow*	main_window;


#include "BPLocale.h"

//generic map

#include <String.h>
#include "Utils.h"

#include "ActionDownload.h"
#include	"DownloadManager.h"

#include "PercentageWindow.h"

extern DownloadManager download_manager;

static KeyMap<BString,OPMLTree*>	fDirectoryTrees;

#define back_color 101,154,206,255

DirectoryWindow::DirectoryWindow(const char* opml_name,BString opml_url):
	BWindow(BRect(350,100,750,700),opml_name, B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS)
{
	fAlert = NULL;
	
	SetSizeLimits(400,1280,150,1024);
	
	fOpmlName.SetTo( opml_name);
	fOpmlURL = opml_url;
	
	
	if(fFolderIcon == NULL)
		fFolderIcon = LoadIcon("folder");  
	
	if(fItemIcon == NULL)
		fItemIcon= LoadIcon("02.png");
	
	BBox	*bgbox=new BBox(Bounds(),"back",B_FOLLOW_ALL,B_WILL_DRAW,B_PLAIN_BORDER);
	AddChild(bgbox);
	
	alist=new BOutlineListView(BRect(15,15,370,530), "ListView1",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE);


	// (240,240,129);
	
	alist->SetLowColor(back_color); //101,154,206);
	alist->SetViewColor(back_color); //101,154,206);    //53,140,42)   101,154,206
	alist->SetHighColor(0,0,0);
	alist->SetFontSize(12);
	
	
	bgbox->AddChild(new BScrollView("PodcastScroll", alist, B_FOLLOW_ALL, 0, false, true));
	
	BBox *viewd=new BBox(BRect(20,551,390,600),NULL, B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT,B_WILL_DRAW,B_NO_BORDER);
	bgbox->AddChild(viewd);

	
	BButton *button1= new BButton(BRect(0,0,100,30),"noname",_T("Collapse All"),new BMessage('refr'), B_FOLLOW_ALL);
	viewd->AddChild(button1);
#ifdef ZETA
	button1->SetToolTipText(_T("Collapse all categories in the directory list"));
#endif	
	/*BButton *button2= new BButton(BRect(130,0,230,30),"noname",_T("Preview"),new BMessage('prev'), B_FOLLOW_ALL);
	 viewd->AddChild(button2);
#ifdef ZETA
	 button2->SetToolTipText(_T("Checkout a podcast before downloading it"));
#endif	*/
	BButton *button3= new BButton(BRect(260,0,360,30),"noname",_T("Subscribe"),new BMessage('subs'), B_FOLLOW_ALL);
	viewd->AddChild(button3);
#ifdef ZETA
	button3->SetToolTipText(_T("Add the selected podcast to the Subscription list"));
#endif	
	alist->SetInvocationMessage(new BMessage('invo'));
	
	PostMessage('refr');
}

void
DirectoryWindow::StartDownload(BString url){
	
	//show the alert
	fAlert= new PercentageWindow("BePodder",_T("Downloading"), LoadIcon("enqueued-32.png"));
	fAlert->Go(this,B_QUIT_REQUESTED);
	entry_ref dest;
	get_ref_for_path(tmpnam(NULL),&dest);
	ActionDownload* action = new ActionDownload(url,dest, false, "", this,'info');

	download_manager.SingleThreadAction(action);	
}

/*void
DirectoryWindow::~DirectoryWindow(){
	
	OPMLParser parser;
	while(fDirectoryTrees.CountItems())
	{
			OPMLTree tree=fDirectoryTrees.ValueAt(0);
			parser.
			RemoveItemsAt(0);
	}
	
}*/

void	
DirectoryWindow::MessageReceived(BMessage *mess2){
 
 		switch(mess2->what){
 			case 'info':
 			DownloadInfo(mess2);
 			break;
			case 'refr':
			{
				//BPath path("http://");
				//path.Append("directories");
				//path.Append(fOpmlName.String());
				
				//BString url("http://www.funkyideasoft.com/directories/bepodder_podcast_opml.tar.gz");
					
				OPMLTree* tree = fDirectoryTrees.ValueFor(fOpmlURL);
				if(tree) {
//					OPMLParser parser;
//					OPMLTree* tree = parser.Parse(path.Path());
					ApplyTree(tree);
//					parser.DeleteOPMLTree(tree);
					}
				else
				StartDownload(fOpmlURL);
				
				/*BPath path(GetAppRelativePath().String());
				path.Append("directories");
				path.Append(fOpmlName.String());
				OPMLParser parser;
				OPMLTree* tree = parser.Parse(path.Path());
				if(tree) 
				{
					ApplyTree(tree);
					parser.DeleteOPMLTree(tree);
				}*/
				//else error!
					
				
				
				
				
				
			}
			break;
			case 'subs':
			{
				DirectoryItem* item=(DirectoryItem*)alist->ItemAt(alist->CurrentSelection());
				if(!item) return;
				if(item->Link() == "") return;
				
				//send message to main_window!
				BMessage	msg('addu');
				msg.AddString("url",item->Link());
				BMessenger(main_window).SendMessage(&msg);
				//BMessenger(this).SendMessage(B_QUIT_REQUESTED);
			}
			break;
			case 'invo':
			{
			//	mess2->PrintToStream();
				int32 index=mess2->FindInt32("index");
				BListItem* item=alist->ItemAt(index);
				if(alist->CountItemsUnder(item,true) > 0){
					if(!item->IsExpanded())
						alist->Expand(item);
					else
						alist->Collapse(item);
				}
				else
				{
					//send message to main_window!
					BMessage	msg('addu');
					msg.AddString("url",((DirectoryItem*)item)->Link());
					BMessenger(main_window).SendMessage(&msg);
				}
					
			}	
			break;
			default:
				BWindow :: MessageReceived(mess2);
			break;
		}
}
void
DirectoryWindow::ApplyTree(OPMLTree* tree){

	alist->MakeEmpty();
	
	if(!tree->foglie) return; //should never happen!
						
//	OPMLParser pa;
//	pa.PrintToStream(tree->foglie->ItemAt(0),0);
	
	_addItem(NULL,tree->foglie->ItemAt(0));
	
	DirectoryItem* item=(DirectoryItem*)alist->ItemAt(0);
	if(!item) return;
	alist->Expand(item);
	alist->Select(0);
	alist->MakeFocus(true);		
}

void	
DirectoryWindow::_addItem(BListItem* root,OPMLTree* foglia){
	
		BBitmap* icon = fFolderIcon;
		BPoint spiaz(0,0);
		
		if(!foglia->foglie)
			icon = fItemIcon;
		else
			spiaz.y=+3;
		
		DirectoryItem *leaf = new DirectoryItem(foglia->description.String(), icon ,foglia->link.String(),spiaz);
		if(root)
		{
			alist->AddUnder(leaf,root);
			if(root->IsExpanded()) alist->Collapse(root);
			LOG("DirectoryWindow", liDebug ,"Added Item [%s] under [%s]",foglia->description.String(),((IconTextItem *)root)->Text());
		}
		else 
		{
			 alist->AddItem(leaf,alist->CountItems());
			 LOG("DirectoryWindow", liDebug ,"Added Item [%s]",foglia->description.String());
	
		}
			
		
			
		if(!foglia->foglie) return;
		
		
		for(int32 i=foglia->foglie->CountItems() -1 ; i>=0 ; i--){
				OPMLTree* subfoglia = foglia->foglie->ItemAt(i);
				_addItem(leaf,subfoglia);
		}
		
}

void
DirectoryWindow::DownloadInfo(BMessage* msg){
			
			ActionDownload::Status status = (ActionDownload::Status)msg->FindInt32("status");
			BString	extra;
			
			switch(status){
			case ActionDownload::OK_DOWNLOADED:
			
			// done.
			
			
			{
			if(fAlert && fAlert->Lock())
				fAlert->Quit();
				//BMessenger(fAlert).SendMessage(B_QUIT_REQUESTED);
			
			
			msg->PrintToStream();
			
			BString filename;
			if(msg->FindString("path",&filename) == B_OK){
					
					
					const char* newName=tmpnam(NULL);
					
					filename.Prepend("tar -xzvOf ");
					filename << " > " << newName;
										
					system(filename.String());
					
					OPMLParser parser;
					OPMLTree* tree = parser.Parse(newName);
					
					unlink(newName);
					msg->FindString("path",&filename);
					unlink(filename.String());
					
					if(tree) {
						ApplyTree(tree);
						BString url;
						if(msg->FindString("url",&url)==B_OK)
							fDirectoryTrees.AddItem(url,tree);
						else
							parser.DeleteOPMLTree(tree);
					}
					
			}
			}
			break;
			
			case ActionDownload::OK_CONNECTING:
				//item->SetFileStatus(CONNECTING);
			break;
			case ActionDownload::OK_PROGRESS:
			{
				
				int32 perc;
//				float speed;
			
				
				if(msg->FindInt32("percentage_progress",&perc)==B_OK){
//					BString per;
//					per << "Downloading " << perc << "%";
//					SetDescription(per.String());	
					if(fAlert) fAlert->SetPercentage(perc);
				}

				
			}
			break;
			
			case ActionDownload::ERROR_PERFORMING:
			{
				
				
				int32 error=msg->FindInt32("curl_error");
				
				switch(error){
					case CURLE_COULDNT_CONNECT:
						SetError(_T("Can't connect!"));
					break;	
					case CURLE_HTTP_PORT_FAILED:
						SetError(_T("Http port failed!"));
					break;
					case CURLE_COULDNT_RESOLVE_HOST:							
						SetError(_T("Can't resolve host!"));
					break;
					case CURLE_HTTP_NOT_FOUND:
						SetError(_T("Not found!"));
					break;
					
					case CURLE_WRITE_ERROR:
					{
						bool	stopped = msg->FindBool("should_stop");
						
						if(!stopped)
							//item->SetFileStatus(STOPPED);
						//else
						{
							//item->SetFileStatus(ERROR);
							SetError("Write error!");
						}
					}
					break;
					
					case CURLE_BAD_DOWNLOAD_RESUME:
						//item->SetFileStatus(ERROR);
						SetError("Bad resume!");					
					break;
					case CURLE_ABORTED_BY_CALLBACK:
						//item->SetFileStatus(ERROR);
						SetError("Aborted by Callback!");					
					break;
					case CURLE_HTTP_RANGE_ERROR:
						//item->SetFileStatus(ERROR);
						SetError("Can't resume! (range error)");
					break;
					case CURLE_FTP_ACCESS_DENIED:
						//item->SetFileStatus(ERROR);
						SetError("Access Denied!");
					break;
					case CURLE_FTP_USER_PASSWORD_INCORRECT:
						//item->SetFileStatus(ERROR);
						SetError("User or Password Incorrect!");
					break;					
					
					default:
					{
						//item->SetFileStatus(ERROR);
						BString ser("Other error (cUrl code: ");
						ser << error << " )";
						SetError(ser.String());
					}
					break;
				}
				
				deletePath(msg);
				
			}
			break;
			
			case ActionDownload::ERROR_CURL_INIT:
				SetError("cUrl init error!");
				deletePath(msg);
			break;
			
			case ActionDownload::ERROR_OPENFILE:
				//item->SetFileStatus(ERROR);
				SetError("Can't open file!");
				deletePath(msg);
			break;						
			default:
			{
				//item->SetFileStatus(ERROR);
				BString ser("Other error (status code: ");
				ser << (long)status << " )";
				SetError(ser.String());
				deletePath(msg);
			}
			break;	
			}
}

void	
DirectoryWindow::deletePath(BMessage* msg){
	BString path;
	if(msg->FindString("path",&path)==B_OK)
				unlink(path.String());
					
}

void
DirectoryWindow::SetError(const char* tex){
			if(fAlert && fAlert->Lock())
				fAlert->Quit();
	
	BString t(fOpmlName);
	t << " : " << tex;
	BPAlert *error=new BPAlert("BePodder",t.String(),"ok",NULL,NULL,B_WIDTH_AS_USUAL, LoadIcon("delete-32.png"));
	error->Go();
	PostMessage(B_QUIT_REQUESTED);
	
}

//void
//DirectoryWindow::SetDescription(const char* tex){
//	if(fAlert && fAlert->Lock()){
//		BString t(fOpmlName);
//		t << " : " << tex;
//		fAlert->TextView()->SetText(t.String());
//		fAlert->Unlock();
//	}
//}
//--
