#include "MainController.h"

#include <Entry.h>
#include <FindDirectory.h>
#include <Directory.h>
#include <Path.h>
#include <NodeInfo.h>
#include <Roster.h>
#include <TranslationUtils.h>
#include <MessageRunner.h>

#include "AttributeExtractor.h"
#include "AttributeDumper.h"
#include "MemoryArchive.h"
#include "XMLChannelExtractor.h"

#include "AttributeMap.h"
#include "ItemProperty.h"

#include "curl/curl.h"

#include "errno.h"

#include "Utils.h"
#include "DownloadManager.h"
#include "MainWindow.h"
#include "Logger.h"

#include "ActionDelete.h"
#include "DownloadListItem.h"
#include "Setting.h"
#include "BPSettingsWindow.h"
#include "BitmapWindow.h" 

#include "BPLocale.h"
#include "EntryIterator.h"

#include "AutoFiletypeMap.h"

#include "BPEpisode.h"

DownloadManager*	download_manager;

//messages:
#define		DOWNLOAD_ITEM				'invD'
#define		DOWNLOAD_CHANNEL			'addu'

#define		VLC_SIGNATURE			"application/x-vnd.videolan-vlc"
#define		SOUNDPLAY_SIGNATURE		"application/x-vnd.marcone-soundplay"
#define		MEDIAPLAYER_SIGNATURE	"application/x-vnd.Haiku-MediaPlayer"


	
//a tendere qui dovrebbe essere
//instaziato il model
MainController::MainController(MainWindow* view, MainModel* model){
	fView = view;
	fModel = model;
	download_manager = new DownloadManager(fView);
	check_time = 0;
	check_timer = NULL;
	check_onstart = false;
	//extraParams.SetTo("");
}

void	
MainController::LoadSettings(const char* type,BMessage* data){

	BString stype(type);
	if(stype.Compare(SETTINGS_PROXY) == 0 ){
		download_manager->LoadProxySetting(data);
	
	}
	else
	if(stype.Compare(SETTINGS_DOWNLOAD) == 0 ){
		download_manager->LoadDownloadSetting(data);
	}
	else
	if(stype.Compare(SETTINGS_FILETYPE) == 0){
			
		browser_id = 3; //System Default.
		player_id  = 3; //Auto.
		int32 ret;
		if(data->FindInt32("browser_index",&ret) == B_OK)
			browser_id = ret;
			
		if(data->FindInt32("player_index",&ret) == B_OK)
			player_id = ret;
	}
	else
	if(stype.Compare(SETTINGS_CHANNELS) == 0 ){
		//one second: 1 000 000;
		bigtime_t minute = 1000000 * 60;
		
		if(check_timer!=NULL) {
			delete check_timer;
			check_timer = NULL;
		}
			
		switch(data->FindInt32("check_time_index"))
		{
			case 0:
				check_time=0;
			break;
			case 1:
				check_time= minute * 5  ; //5 mins
			break;
			case 2:
				check_time= minute * 15 ; //15 mins
			break;
			case 3:
				check_time= minute * 30; //30 mins
			break;
			case 4:
				check_time=minute * 60; //1h
			break;
			case 5:
				check_time= minute * 120; //2h
			break;
		}
		
		check_onstart = data->FindBool("check_startup");
		
		if(check_time>0)
			printf("Check every %lld microseconds..\n",check_time);
		else
			printf("Automatic check disabled\n");
		
		if(check_time>0)
				check_timer= new BMessageRunner(fView,new BMessage(CHECK_ALL),check_time);
	}
}

/*
void	
MainController::DownloadSelectedItem(){

	//ANNOTAZIONI: se si lascia solo DownloadItem(REF) si può pensare alla multiselezione.

	EpisodeListItem* sel = fView->GetSelectedEpisode();
	if(!sel) return;
	
	
	if(DownloadItem( sel->fRef,sel )==B_OK) {
	}
	
	//QUI: inserire il collegamento con SEL e Refresh dell' episodio.
}
*/
status_t
MainController::DownloadItem(entry_ref ref, EpisodeListItem* epi) {
			
			//start a item download.
			//------------------------------------------------------------------
			
			//download view (only if there's already one)
			DownloadListItem* item=fModel->findDownload(ref);
			
			BString enclosure_path;
			BString enclosure_link;
			BString enclosure_type = "";
			FileStatus status;
			
			if(item) 
			{	
				enclosure_path=item->fRss.EnclosureLocalPath();
				enclosure_link=item->fRss.EnclosureLink();
				enclosure_type=item->fRss.FileType();
				status=item->fRss.GetFileStatus();
			}
			else
			{
				BPEpisode bpe(&ref);
				if(bpe.InitCheck()!=B_OK) return B_ERROR;
				enclosure_path=bpe.EnclosureLocalPath();
				enclosure_link=bpe.EnclosureLink();
				enclosure_type=bpe.EnclosureType();
				status= bpe.GetStatus();
			}
						
			if(enclosure_path == "" || enclosure_link == "") return B_ERROR;
			
			
						
			bool resume = false;
			 
			if( 	status == NOT_DOWNLOADED || 
					status == STOPPED ) 
			{
			
			 resume = (status == STOPPED );
			 
			 BPath path(&ref);
			 if(path.GetParent(&path)!=B_OK) return B_ERROR;
			 
			 BDirectory dir; 
			 path.Append("enclosures");
	
			 if(dir.SetTo(path.Path()) == B_ENTRY_NOT_FOUND)
			  	if(dir.CreateDirectory(path.Path(),&dir)!=B_OK) return B_ERROR;
			 
			 
			 if(dir.InitCheck()!=B_OK) return B_ERROR;
			 		
			 path.Append(enclosure_path.String());		
			 
			 entry_ref enclosure_ref;
			 get_ref_for_path(path.Path(),&enclosure_ref);
			 ActionDownload *ac=new ActionDownload(enclosure_link,enclosure_ref,resume, enclosure_type);	
			 
			 ac->SetRef("episode_ref",&ref);
			 
				
			
			 
			 if( item ==NULL) {
			 	item = new DownloadListItem(NULL);
				AttributeExtractor an;
				//HERE: we have to move data from ref to item->fRss.
				an.SetNotifier(&item->fRss);
				BNode bpe(&ref);
				an.Parse(&bpe);
				
				item->fRef=ref;
				fModel->addDownload(ref,item);
				
				if(epi)
			 		item->LinkToEpisode(epi);
			 	
				fView->AddDownload(item);
								
			 }
			 
			 ac->SetExtraData(item->fRss.Guid());
			 item->fRss.SetFileStatus(ENQUEQUED);
			 download_manager->Enqueue(DownloadManager::ITEMS_QUEUE,ac);
			
			return B_OK;
	}
	
	return B_OK;
}

void	
MainController::InfoDownloadItem(BMessage* msg){
			
			entry_ref ref;
			if( msg->FindRef("episode_ref",&ref) != B_OK) return;

			DownloadListItem*		item=fModel->findDownload(ref);
			if(!item) return;			

			ActionDownload::Status status = (ActionDownload::Status)msg->FindInt32("status");

			RssItem* rss = &item->fRss;
			if(!rss) return;
			
			switch(status){
			case ActionDownload::OK_DOWNLOADED:
			{
				
				AttributeDumper dumper(&ref);
					
				rss->AddElementListener(&dumper);		
				rss->SetFileStatus(DOWNLOADED);//actually save the rss-file				
				rss->RemoveElementListener(&dumper);
				
				//FIX: test questo if.
				if(fModel->fSelectedItem.InitCheck()==B_OK && fModel->fSelectedItem == BNode(&ref) ) {
					UpdateSelected();
				}
			}
			break;
			case ActionDownload::OK_CONNECTING:
				rss->SetFileStatus(CONNECTING);
			break;
			case ActionDownload::OK_PROGRESS:
			{
				
				int32 perc;
				if(msg->FindInt32("percentage_progress",&perc)==B_OK)	{
					item->fRss.SetDownloadSpeed(msg->FindFloat("download_speed"));
					item->fRss.SetFilePercentage(perc);
				}			
				
				rss->SetFileStatus(DOWNLOADING);
				
				//item->SetFilePercentageAndSpeed(perc,msg->FindFloat("download_speed"));
			}
			break;
			
			case ActionDownload::ERROR_PERFORMING:
			{
				
				int32 error=msg->FindInt32("curl_error");
				
				switch(error){
					case CURLE_COULDNT_CONNECT:
					case CURLE_HTTP_PORT_FAILED:
					case CURLE_COULDNT_RESOLVE_HOST:							
						rss->SetFileStatus(CANT_CONNECT);
					break;
					case CURLE_HTTP_NOT_FOUND:
						rss->SetFileStatus(NOT_FOUND);
					break;
					
					case CURLE_WRITE_ERROR:
					{
						bool	stopped = msg->FindBool("should_stop");
						
						if(stopped){
							AttributeDumper dumper(&ref);
							rss->AddElementListener(&dumper);		
							rss->SetFileStatus(STOPPED);	//actually save the rss-file				
							rss->RemoveElementListener(&dumper);
						}
						
						if(item->fShouldMeRemoved)
							RealRemoveDownload(item);
						
					}
					break;
					
					case CURLE_BAD_DOWNLOAD_RESUME:
					case CURLE_ABORTED_BY_CALLBACK:
					
					default:
						rss->SetFileStatus(ERROR);
					break;
				}
			}
			break;
			
			case ActionDownload::ERROR_CURL_INIT:
			case ActionDownload::ERROR_OPENFILE:
			default:
				rss->SetFileStatus(ERROR);
			break;	
			}
}


status_t
MainController::StopDownloadItem(entry_ref ref){
	
	DownloadListItem* item=fModel->findDownload(ref);
	if(!item) return B_ERROR;

	BString guid(item->fRss.Guid());
	if(guid == "") return B_ERROR;
	
	FileStatus status = item->fRss.GetFileStatus();
	
	//------------------------------------------------------------------

	if(status== DOWNLOADING){
			download_manager->TryStopCurrentAction(DownloadManager::ITEMS_QUEUE,"extra",guid);
	}
	else
	if(status == ENQUEQUED){
		bool result=download_manager->RemoveFromQueue(DownloadManager::ITEMS_QUEUE,"extra",guid);
		if(result)
			item->fRss.SetFileStatus(NOT_DOWNLOADED); //this iclude also refresh of donwload item AND (if linked) the episodeListItem.
	}
	
	return B_OK;
}



status_t
MainController::PlayItem(entry_ref ref){

	BPEpisode bpe(&ref);
	
	if(bpe.InitCheck()!=B_OK) return B_ERROR;
	
	BString enclosure_path(bpe.EnclosureLocalPath());
	
	if(enclosure_path == "" ) 
				WWWItem(ref);
	
	//------------------------------------------------------------------
	 BPath path(&ref);
	 if(path.GetParent(&path)!=B_OK) return B_ERROR;
	 
	 BDirectory dir; 
	 path.Append("enclosures");

	 if(dir.SetTo(path.Path()) == B_ENTRY_NOT_FOUND) return B_ERROR;
	 
	 path.Append(enclosure_path.String());
	  		
	  BString filetype(bpe.EnclosureType());			 
	  OpenFile(path.Path(),filetype);
	  
  	return B_OK;
}

void
MainController::ShowChannelImage(entry_ref ref){

	// Use Full resolution image saved as a file
	BDirectory dir(&ref); //channel
	if(dir.InitCheck()!=B_OK) return;
	BPath path(&dir,"image");

	entry_ref image_ref;
	get_ref_for_path(path.Path(),&image_ref);

	BBitmap	*icon = BTranslationUtils::GetBitmap(&image_ref);
	if(!icon) return;
	(new BitmapWindow(icon))->Show();
}


void
MainController::SetSelectedItem(entry_ref* ref,entry_ref *nref){
	
	//debugger("SEL");		
	
	if(BNode(ref) == fModel->fSelectedItem &&
	   BDirectory(nref) == fModel->fSelectedChannel ) {
	   	return;
	} 
	
	 if(nref)
		fModel->fSelectedChannel.SetTo(nref);
	else
		fModel->fSelectedChannel.Unset();
		  
	if(ref)	
		fModel->fSelectedItem.SetTo(ref);
	else
		fModel->fSelectedItem.Unset();
		   
	
	SubscriptionListItem*	row=fView->GetSelectedSubscription();
	if(!row) return;

	EpisodeListItem* sel = fView->GetSelectedEpisode();
				
	if(sel && sel->IsNew()){
			BPEpisode bpe(&sel->fRef);
			bpe.AddElementListener(sel);
			BString enclosure(bpe.EnclosureLink());
			if(enclosure != "")		
				bpe.SetStatus(NOT_DOWNLOADED);
			else
				bpe.SetStatus(NO_ENCLOSURE);	
							
			row->AddNewCount(-1); 
			fView->RefreshSubscription(row);
	}

	UpdateSelected();			
}

void	
MainController::UpdateSelected() {
	
	//reloads the MemoryArchive and update view.
	MemoryArchive* archive=&fModel->fCurrentSelectedItem;
	
	archive->MakeEmpty();
	
	AttributeExtractor extractor;
	freeNotifier.AddElementListener(archive);
	extractor.SetNotifier(&freeNotifier);
	
	if(fModel->fSelectedChannel.InitCheck()==B_OK){
		extractor.Parse(&fModel->fSelectedChannel,true);
	}
	
	if(fModel->fSelectedItem.InitCheck()==B_OK) {
		extractor.Parse(&fModel->fSelectedItem,true);
	}

	freeNotifier.RemoveElementListener(archive);		
	fView->ShowDescription(archive);	
}


/*
void
MainController::SetSelectedChannel(BString url){
	
			ChannelInfo*	ci = fModel->fChInfo.ValueFor(url);
			if(!ci) return; 
			
			if(!ci->row) return;
			
			EpisodeListView* dx_list= ci->elv;
			if(!dx_list) return;
			
			EpisodeListItem* sel = (EpisodeListItem*)dx_list->CurrentSelection();
			
			fModel->fCurrentSelectedChannel.MakeEmpty();
			MemoryArchive* archive=&fModel->fCurrentSelectedChannel;
			
			AttributeExtractor extractor;
			freeNotifier.AddElementListener(archive);
			extractor.SetNotifier(&freeNotifier);
			
			
			if(ci->row!=NULL){
					BDirectory dir(&ci->nref);
					if(dir.InitCheck()==B_OK){
						extractor.Parse(&dir,true);
					}
			}
			
			freeNotifier.RemoveElementListener(archive);		
			//fView->ShowDescription(archive);	
}
*/

status_t	
MainController::FindItem(entry_ref ref){
	
	//FIX DA CONTROLLARE
	
	BPath path(&ref);
	path.GetParent(&path);
	entry_ref parent;
	get_ref_for_path(path.Path(),&parent);
	
	int32 count=fModel->fChInfo.CountItems();
	SubscriptionListItem*		row=NULL;
	
	for(int i=0;i<count;i++){
		row=fModel->fChInfo.ValueAt(i);
		BDirectory dir(&row->fRef);
		if( BNode(&dir,NULL) == BNode(&parent))
			break;
		else
			row=NULL;
	}
	
	if(!row) return B_ERROR;
	
	EpisodeListView* dx_list= fView->GetEpisodeListView();
	if(!dx_list) return B_ERROR;
	
	
	//  FIX come faccio a selezionare questo?
	//  dovrei fare una bella cosa: se è la view visibile allora lo seleziono,
	//  altrimenti bisogna notificare a fView di selezionare il canale E di preparare quel ref per essere
	//  il selezionato.
				
	//dx_list->DeselectAll();
	//dx_list->AddToSelection(sel);
	fView->SelectSubscription(row);
	return B_OK;
}


void
MainController::WWWItem(entry_ref ref){

	BPEpisode bpe(&ref);
	
	BString link=bpe.EnclosureLink();
	if(link == "") return;
	
	OpenURL(link);
}

bool
MainController::DeleteEpisode(entry_ref ref){

	//FIX: da testare!
	
	DownloadListItem*	item = fModel->findDownload(ref);
	
	if(item){
		StopDownloadItem(ref);
		RealRemoveDownload(item);
	}
	
	//cancellare l'enclosure
	BPEpisode bpe(&ref);
	FileStatus 	status=bpe.GetStatus();
	BString enclosure_path=bpe.EnclosureLocalPath();
	if(enclosure_path != ""){
	    BPath path(&ref);
	    if(path.GetParent(&path)==B_OK){
	    	path.Append("enclosures");
	 		path.Append(enclosure_path.String());
	 		unlink(path.Path());	
	    }
	}	
	
	//cancellare l' file-item
	BPath file_item(&ref);
	unlink(file_item.Path());
	
	//FIX: attenzione questo metodo non 'tocca'
	// la grafica (fView) perchè lo fa invece la action..
	// si discosta dal resto.. ma è sbagliato, o forse anche
	// altre action dovrebbero essere così 'agressive' ?
	return (status == NEW );
}


status_t
MainController::RemoveDownload(entry_ref ref){

	DownloadListItem* dli=fModel->findDownload(ref);
	if(!dli || dli->fShouldMeRemoved) return B_ERROR;
	
	dli->fShouldMeRemoved=true;
	StopDownloadItem(ref);
	return B_OK;
}

void	
MainController::RealRemoveDownload(DownloadListItem* dli){
	dli->Unlink();
	fModel->removeDownload(dli->fRef);	
	fView->RemoveDownload(dli);
}


const MemoryArchive*
MainController::GetSelectedItem(){
	return &fModel->fCurrentSelectedItem;
}

void
MainController::FolderSelectedChannel(entry_ref nref){

	//use preferred application.
	BString app = PathFolderChannel(nref);	
	const char *args[] = { app.String(), 0 };
	be_roster->Launch("application/x-vnd.Be-directory", 1, const_cast<char **>(args));
	printf("trying to open %s\n", app.String());
}


BString	
MainController::PathFolderChannel(entry_ref nref){

	SubscriptionListItem*	row=fModel->fChInfo.ValueFor(nref);
	if(!row) return "impossible";
					
	BPath path(&row->fRef);
	path.Append("enclosures");
	
	BEntry check(path.Path());
	if(check.InitCheck()== B_OK && !check.Exists())
		path.SetTo(&row->fRef);

	return path.Path();
}

void	
MainController::DownloadChannel(BMessage* msg){
				
	BString	url;
	if(msg->FindString("url",&url)!=B_OK || url == "" ) return;
	
	//FIX test di url già presente!
	int32 count=fModel->fChInfo.CountItems();
	for(int32 i=0;i<count;i++)
		if(fModel->fChInfo.ValueAt(i)->GetURL().ICompare(url)==B_OK) return; //URL già presente!
		
	/*if(!linfo.Valid() && count > 10)
	{
		
		BString text(_T("You can only add up to 10 channels in this unregistred BePodder version.\n Please register your copy\n"));	
		BPAlert* about = new BPAlert("Sorry", text.String() ,_T("Web Site"),_T("Close"),NULL,B_WIDTH_AS_USUAL,LoadIcon("logo-64.png"));
		int ret=about->Go();
		if(ret==0){
			OpenURL("http://www.funkyideasoft.com/bepodder.html");
		}
		return;
	}*/
					
	
	AttributeDumper dumper(&fModel->archive_ref,true);
	entry_ref nref;
	dumper.GetEntryRef(&nref);

	SubscriptionListItem* row= new SubscriptionListItem(nref);
	fView->AddSubscription(row,NULL);
	fView->ScrollToSubscripion(row);
	row->SetPreferredPosition(fView->IndexOfSubscription(row));
					
	RssChannel* channel = &row->fChannel;
	channel->AddElementListener(&dumper); //in order to save as attribute
			
	channel->SetTitle(url.String());
	channel->SetURL(url.String());
	channel->SetFileStatus(NOT_DOWNLOADED);
	
	channel->RemoveElementListener(&dumper);
	
	fModel->fChInfo.AddItem(nref,row);
	CheckChannel(nref);
}

void	
MainController::InfoDownloadChannel(BMessage* msg){
		
			//info di download di un channel.
			
			
			entry_ref nref;
			
			if(msg->FindRef("channel_ref",&nref)!=B_OK) return;
			
					
			SubscriptionListItem* row = fModel->fChInfo.ValueFor(nref);
			if(!row) return;
			
			
			RssChannel* channel= &row->fChannel;
			
			
			ActionDownload::Status status = (ActionDownload::Status)msg->FindInt32("status");
				
			//LOG("MainController",liDebug,"InfoChannel status %ld",status);
			
			
			switch(status){
				
			
				
			case ActionDownload::OK_DOWNLOADED:
			{
			
				MemoryArchive	c_archive;
				AttributeDumper dumper(&row->fRef);
				
				channel->AddElementListener(&dumper);		//update the channel file.
				channel->AddElementListener(&c_archive);	//temp info in memory.	
				 
				channel->SetFileStatus(DOWNLOADED);
				
				BString path;
				msg->FindString("path",&path);
								
				XMLChannelExtractor ext;
				ext.SetNotifier(channel);
				BList *list = ext.Parse(path.String());
				
				if(!list) {
					
					channel->SetFileStatus(BAD_FORMAT);
					channel->RemoveElementListener(&dumper);
					channel->RemoveElementListener(&c_archive);
					 
					fView->RefreshSubscription(row);
					return; //check (delete di qualcosa?)
				}
				
				
				// come procedere (non mi delizia la cosa)
				// abbiamo N files nella directory e K file nella lista.
				// scorriamo UNA VOLTA gli N file e leggiamo l'attributo GUID.
				// lo confrontiamo con tutti gli elementi K.
				// se match, lo eliminiamo dalla lista se no lo lasciamo.
				BDirectory dir(&row->fRef);
				EntryListBase *result = new CachedDirectoryEntryList(dir);	
				
				result->Rewind();
				entry_ref ref;
				
				//FIX:
				/*
				bool delNew=false;
				bool delOld=false;
						
				if(row->extraSetting) {
					BMessage del;
					if(row->extraSetting->FindMessage("channel_setting",&del) == B_OK){
						del.FindBool("what_delete_new",&delNew);
						del.FindBool("what_delete_read",&delOld);
						del.PrintToStream();
					}
				}
				
				int32 removed=0;
				bool forceUpdate=false;		
				*/
									
				while( list->CountItems() > 0  && result->GetNextRef(&ref)  == B_OK ){
					
					BPEpisode bpe(&ref);
				
					//qui ho diritto di vita o di morte sul file selezionato.. prima cosa riepilogo i settaggi che ho:
						//bene, già qui abbiamo un problema: come ottengo quello che non ho?
						//bha proviamo un pò..
						//debugger(";)");
						//if( (delNew && bpe.GetStatus() == NEW ) ||
						/*if	(delOld && bpe.GetStatus() != DOWNLOADED ) {
						
							
							if(DeleteEpisode(ref)) removed--;
							forceUpdate = true;
							
						}*/
				
				
					BString guid(bpe.Guid());
					if(guid != "") {						
						int32 newCount =  list->CountItems();
						for(int i=newCount - 1 ;i >= 0 ;i--){
							MemoryArchive	*ma =(MemoryArchive*)list->ItemAt(i);
							if(ma->GetDataString(ITEM_GUID).Compare(guid) == 0) {
								list->RemoveItem(ma); 
								delete ma;
								// break; // meglio toglierlo in caso nella lista esistano + elementi con lo stesso GUID (raro)
							}
						}
					
						
					
					}
				}
				
				//FIX if(removed!=0)
				//	row->AddNewCount(removed);
					
				
				int added=0;
				
				while(list->CountItems()){
				
					MemoryArchive	*ma =(MemoryArchive*)list->ItemAt(0);
					//we have it, removes from the list:
					list->RemoveItem(ma); 
					
					
					BString	guid(ma->GetDataString(ITEM_GUID));
					
					RssItem fake;
					EpisodeListItem *ep=NULL;
					
					//new EPISODE file:
					AttributeDumper dumper(&row->fRef,false);
					fake.AddElementListener(&dumper);
					
							
					if(row == fView->GetSelectedSubscription()) {
							ep=new EpisodeListItem();
							fake.AddElementListener(ep);
					}			
					
					ma->FillListener(&fake); //save to file
					
					fake.RemoveElementListener(&dumper);
							
					if(ep){
						fake.RemoveElementListener(ep);
						dumper.GetEntryRef(&ep->fRef);
						fView->GetEpisodeListView()->AddRow(ep,0,(BRow*)NULL);	
							
					}
					added++;
				 }
				 
				 delete list;
				 		 				
				 channel->RemoveElementListener(&dumper);
				 channel->RemoveElementListener(&c_archive);		
				 
				 if(added > 0) {
				 		row->SetIsNew(true);
				 		channel->SetKeyInt32(CHANNEL_NEWITEMS,added);
//FIX:ask?				 		dx_list->Refresh();
				 }
				 
				 //we can check for an image link and maybe add the the LowPrority queue.
				 
				 BString imageLink(c_archive.GetDataString(CHANNEL_IMAGE_URL));
				 if(imageLink != "" && !row->HasOwnImage())
				 {
				 	
				 	 BDirectory dir; //channel
					 if(dir.SetTo(&row->fRef)!=B_OK) return;
					 
					 BPath path(&dir,NULL); //path to channel
			 		
			 		 path.Append("image");
	
					 entry_ref ref;
			 		 get_ref_for_path(path.Path(),&ref);
				 	
				 	ActionDownload* ac= new ActionDownload(imageLink,ref,false, "image");
				  	
				   	ac->SetRef("channel_ref",&row->fRef);
			 		ac->SetExtraData("image");
			 		
			 		//low->AddAction(ac);
			 		download_manager->Enqueue(DownloadManager::EXTRA_QUEUE,ac);
				 }
				 
				 //final touch; if this channel is the selected one then update Info panel
				 if(row == fView->GetSelectedSubscription()){
				 	//FIX
				 	/*	if(forceUpdate) {
				 			è sbagliato tra laltro (mi sembra!)
				 			fModel->fSelectedChannel.Unset();
				 			fModel->fSelectedItem.Unset();		
				 			SwitchChannel(&row->fRef);
				 		}
				 		else*/
			 				fView->ShowDescription(&fModel->fCurrentSelectedItem);	
			 			
				 }
				 

			}
			break;
			case ActionDownload::OK_PROGRESS:
			{
				channel->SetFileStatus(DOWNLOADING);
				int32 perc = 0;
				if(msg->FindInt32("percentage_progress",&perc)==B_OK)			
				{
					row->SetFilePercentage(perc);
				}
			}
			break;
			case  ActionDownload::OK_CONNECTING:
				channel->SetFileStatus(CONNECTING);
			break;
			case ActionDownload::ERROR_PERFORMING:
			{
				int32 error=msg->FindInt32("curl_error");
				
					
				switch(error){
					case CURLE_COULDNT_CONNECT:
					case CURLE_HTTP_PORT_FAILED:			
					case CURLE_COULDNT_RESOLVE_HOST:		
						channel->SetFileStatus(CANT_CONNECT);
					break;
					case CURLE_HTTP_NOT_FOUND:
						channel->SetFileStatus(NOT_FOUND);
					break;
					default:
						channel->SetFileStatus(ERROR);
					break;
				}
			}
			break;
			default:
				channel->SetFileStatus(ERROR); //TODO: more verbose.
			break;
			
			} //end switch
			
			fView->RefreshSubscription(row);
}


void
MainController::ParseArchive()
{
	BPath path;
	find_directory (B_USER_CACHE_DIRECTORY, &path, true);
	path.Append("BePodder/archive");

	create_directory( path.Path(), 755 );

			
	BEntry e(path.Path(),true);
	
	if(e.InitCheck() !=B_OK) {
		be_app->PostMessage(B_QUIT_REQUESTED);
		printf("No valid archive directory! (%s) \n",path.Path());
		return;
	}
	
	//se la directory non esiste cerca di crearla.
	
	if(!e.Exists()){
		BDirectory archive;
		if(	archive.CreateDirectory(path.Path(),&archive) != B_OK)	{
				be_app->PostMessage(B_QUIT_REQUESTED);
				printf("Can't create archive directory! (%s) \n",path.Path());
				return;
		}
	}		
	
	e.GetRef(&(fModel->archive_ref));
				
	uint32 flags=0;

	if(fView->Lock()){
		flags	 = fView->Flags();
		fView->SetFlags( flags | B_NOT_RESIZABLE);
		fView->Unlock();
	}
	
	bigtime_t start = real_time_clock_usecs();
	

	BDirectory dir(&fModel->archive_ref);
	
	//LOAD GROUPS:
	
	BMessage groups;
	ReadAttributeMessage(&dir,ATTR_GROUPS,&groups);
	LoadGroupSetting(&groups);
	
	//LOAD CACHE:
	
	BMessage cache;
	ReadAttributeMessage(&dir,ATTR_CACHE,&cache);
		
	BEntry		entry;
	while(dir.GetNextEntry(&entry) == B_OK) 
		if(entry.InitCheck()==B_OK && entry.IsDirectory()){
				CreateChannelItem(&entry,&cache);
		}
	
	bigtime_t stop = real_time_clock_usecs();
	
	printf("Parsing time %lld secs. \n",(stop-start)/1000000);
	
	cache.MakeEmpty();
	
	WriteAttributeMessage(&dir,ATTR_CACHE,&cache);
	
	if(fView->Lock()){
		fView->SetFlags( flags );
		fView->Unlock();
	}
	
	if(check_onstart)
		fView->PostMessage(new BMessage(CHECK_ALL));
	
}

int32
CountNewItems(entry_ref nref){
	
	BDirectory dir(&nref);
	dir.Rewind();
	const char* name="bp:item_status";
	int32 total = 0;
	
	entry_ref ref;
	while(dir.GetNextRef(&ref) == B_OK) {
		BNode node(&ref);

		status_t status = node.InitCheck();
		if (status < B_OK) continue;
		int32 dest;
		if( ReadAttributeInt32(&node,name,&dest) == B_OK && dest==8)
			total++;
	}
	return total;
}


void	
MainController::CreateChannelItem(BEntry* e,BMessage* cache)
{
	BNode node(e);
	
	if(!CheckMIME(MIME_CHANNEL,&node)) return;
	
	entry_ref nref;
	e->GetRef(&nref);
	
	SubscriptionListItem *row=new SubscriptionListItem(nref);
	
	//parsing degli attribute..
	
	AttributeExtractor an;
	an.SetNotifier(&row->fChannel);
	an.Parse(&node);
	
	//--- c'e' L'url? se no delete di tutto e ciao! ---//
	
	if(row->GetURL() == "") {
			delete row;
			return;
	}
	
	
	//check delle info di cache..
	int64 when=1;
	time_t now=0;
	if(node.GetModificationTime(&now)!=B_OK) now=0;
	
	BString key=row->GetURL();
	key << "_count";
	int32 count;
			
	if( cache->FindInt32(key.String(),&count)!=B_OK || 
		cache->FindInt64(row->GetURL().String(),&when)!=B_OK || (time_t)when!=now) {
			printf("Recaching for.. %s\n",row->GetURL().String());
			count=CountNewItems(row->fRef);
		}
	
	row->AddNewCount(count);
				
	BMessage data;
	if(	ReadAttributeMessage(&node,ATTR_COLUMNS_STATE,&data) == B_OK)
					row->columnsState = new BMessage(data);

	data.MakeEmpty();
	GroupItem* group=NULL;
	if(	ReadAttributeMessage(&node,ATTR_EXTRA_STATE,&data) == B_OK)
					group=LoadExtraState(&data,row);


	if(fView->Lock()){
		fView->SetSortingSubscriptions(true);
		fView->AddSubscription(row,group);
		fModel->fChInfo.AddItem(row->fRef,row);
		fView->SetSortingSubscriptions(false);
		fView->Unlock();
	}
	
}

void	
MainController::OpenFile(const char* filename,BString filetype){

		 	entry_ref ref;
			get_ref_for_path(filename,&ref);
			 
			BMessage refs(B_REFS_RECEIVED);
			refs.AddRef("refs",&ref);
			
			
			int b = -1;
			
			BString alert;		
			
			if(player_id ==3) //auto
			{	
				b = FindBestApplication(&ref,alert);
				
				if(alert != "") {
					BPAlert *pop= new BPAlert("BePodder", alert.String() ,_T("Ok"),NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("logo-64.png"));
					pop->Go();
				}
			}
			else
			{
				
				if(filetype == "" || (filetype.FindFirst("audio/") >= 0 )
							    	   || (filetype.FindFirst("video/") >= 0 ) )
							    	   			b = player_id;
			}			
			
			switch(b){
				case -1:
					//use preferred application.
					be_roster->Launch(&ref);
				break;
				case 0:
					if(be_roster->Launch(MEDIAPLAYER_SIGNATURE,&refs)!=B_OK)
						AlertBadPlayer(MEDIAPLAYER);
				break;
				case 1:
					if(be_roster->Launch(SOUNDPLAY_SIGNATURE,&refs) !=B_OK)
						AlertBadPlayer(SOUNDPLAY);
				break;
				case 2:
					if(be_roster->Launch(VLC_SIGNATURE,&refs) !=B_OK)
						AlertBadPlayer(VLC);
				break;
				default:
				break;
			};	
}


void
MainController::AlertBadPlayer(int32 p_id){
	
	// Use this with HaikuDepot?
	/*
	BString text(_T("Can't open the selected file with:\n\n"));
	BString url;
	
	if ( p_id == MEDIAPLAYER)
	{
		text << "MediaPlayer";
		url.SetTo("http://www.haiku-os.com/");
	}
	else
	if ( p_id ==VLC)
	{
		text << "VideoLAN";
		url.SetTo("http://www.bebits.com/app/2119");
	}
	else
	if(p_id==SOUNDPLAY)
	{
		text << "SoundPlay";
		url.SetTo("http://www.bebits.com/app/156");
	}
		
	text << _T("\n\nPlease check if the application is installed in the right way.\n");
	

	BPAlert *pop= new BPAlert("BePodder", text.String() ,_T("Download it!"),_T("Close"),NULL,B_WIDTH_AS_USUAL,LoadIcon("logo-64.png"));
	int32 ret=pop->Go();
	if(ret==0)
		OpenURL(url);
	
		
	*/
}


void	
MainController::OpenURL(BString link){

		BString app("text/html");
		bool lunched = false;
	
		if ( browser_id == 0 ) //WebPositive.
		{
			app.SetTo("application/WebPositive");
			entry_ref ff;
			if(be_roster->FindApp("application/x-vnd.Haiku-WebPositive",&ff) == B_OK) {
				BString path(BPath(&ff).Path());
				path.RemoveLast("-bin");
				path << " " << link << " &";
				system(path.String());
				lunched = true;
			}

		}
		else
		if ( browser_id == 1 ) // QupZilla
		{
			app.SetTo("application/QupZilla");
			entry_ref ff;
			if(be_roster->FindApp("application/x-vnd.qt5-qupzilla",&ff) == B_OK) {
				BString path(BPath(&ff).Path());
				path.RemoveLast("-bin");
				path << " " << link << " &";
				system(path.String());
				lunched = true;
			}
   		}
   		else
   		if ( browser_id == 2 ) // NetSurf
		{
			app.SetTo("application/NetSurf");
			entry_ref ff;
			if(be_roster->FindApp("application/x-vnd.NetSurf",&ff) == B_OK) {
			
				BString path(BPath(&ff).Path());
				path.RemoveLast("-bin");
				path << " " << link << " &";
				system(path.String());
				lunched = true;
			}
 		}
		if (browser_id == 3 || lunched == false) // Default
		{
			app.SetTo("text/html");
			const char *args[] = { link.String(), 0 };
   			be_roster->Launch (app.String(), 1, const_cast<char **>(args));
   		}
}

void
MainController::SaveGroupSetting(BMessage* grp){
	for(int32 i=0;i< fModel->fGroups.CountItems();i++) {
		GroupItem* group=fModel->fGroups.ValueAt(i);
		grp->AddString("group_name",group->GroupName());
		grp->AddBool("group_expanded",group->IsExpanded());
	}
}

void	
MainController::LoadGroupSetting(BMessage* grp){
	int i=0;
	BString name;
	bool expanded;
	
	while(grp->FindString("group_name",i,&name)==B_OK){
		grp->FindBool("group_expanded",i++,&expanded);
		
		AddSavedGroup(name.String(),expanded);
		//FIX APPLY EXPAND OR NOT!
	}
}


void
MainController::SaveColumnsState(BMessage* msg){
	
	EpisodeListView*	elv=fView->GetEpisodeListView();
	
	msg->MakeEmpty();
	msg->AddRect("view_rect",elv->Bounds());
	
	int32 count = elv->CountColumns();
	
	for(int i=0;i<count;i++){
		BColumn *col = (BColumn*) elv->ColumnAt(i);
		msg->AddFloat("width",col->Width());
		msg->AddBool("visible",col->IsVisible());
		msg->AddInt32("position",col->LogicalFieldNum());
	}
	
	BMessage superstate;
	elv->SaveState(&superstate);
	msg->AddMessage("state",&superstate);
		
}
	
void
MainController::LoadColumnsState(BMessage* msg){

	EpisodeListView*	elv=fView->GetEpisodeListView();
	elv->SetEditMode(true);	
	
	BRect bounds = msg->FindRect("view_rect");
	if(bounds.top!=0 || bounds.left!=0 || bounds.right <= bounds.left) return; // safety?
	
	int32 count = elv->CountColumns();
	
	for(int i=0;i<count;i++){
		
		int32 position;
		if(msg->FindInt32("position",i,&position) != B_OK)
			position = i;
						
		BColumn *col = (BColumn*) elv->ColumnAt(position);
		elv->SetColumnVisible(col,false);
		
		float w;
		if(msg->FindFloat("width",i,&w)==B_OK)
						col->SetWidth(w);
			
		bool visible;
		
		if(msg->FindBool("visible",i,&visible)== B_OK)
			elv->SetColumnVisible(col,visible);
		else
			elv->SetColumnVisible(col,true);
			
	}
	
	
	BMessage superstate;
	if(msg->FindMessage("state",&superstate)==B_OK)
				elv->LoadState(&superstate);
	
	// TODO FIX ME
	/*
	elv->GetTitleView()->ScrollTo(BPoint(bounds.Width(),0));
	elv->GetTitleView()->ScrollTo(BPoint(0,0));
	*/
	elv->ScrollView()->ScrollTo(BPoint(0,0));
	elv->SetEditMode(false);	
		
}

void
MainController::SaveExtraState(BMessage* msg, SubscriptionListItem* row){
	
	int32 index = fView->IndexOfSubscription(row);
	msg->AddInt32("preferred_index", index);
	BString group=fView->GroupOf(row);
		
	if(group !="")
		msg->AddString("group_name", group);
}
	
GroupItem*
MainController::LoadExtraState(BMessage* msg, SubscriptionListItem* row){
	int32 index = -1;
	if(msg->FindInt32("preferred_index",&index) !=B_OK) index=-1;
	row->SetPreferredPosition(index);
	
	BString group;
	if(msg->FindString("group_name", &group)==B_OK) 
			return AddSavedGroup(group.String(),true);

	return NULL;
}

void	
MainController::ArchiveChannel(entry_ref nref){
	TRACE_CLASS;

	SubscriptionListItem* 	row = fModel->fChInfo.ValueFor(nref);
	if(!row) return;
	
	BDirectory dir(&row->fRef);
	BNodeInfo(&dir).SetType(MIME_ARCHIVED_CHANNEL);
	
	//remove from gui.
	RemoveChannelFromUI(nref,row);
}

status_t	
MainController::DeleteChannel(entry_ref nref){
	
	//FIX DA TESTARE
	
	//cancellare e togliere tutti i download che hanno un ref nel canale.
	
	SubscriptionListItem* 	row = fModel->fChInfo.ValueFor(nref);
	if(!row) return B_ERROR;
			
	BDirectory dir(&nref);
	EntryListBase *result = new CachedDirectoryEntryList(dir);	
	
	result->Rewind();
	entry_ref ref;

	
	while(result->GetNextRef(&ref)  == B_OK ){
		DownloadListItem*	item = fModel->findDownload(ref);
		if(item)
				RealRemoveDownload(item);
	}
	
	
	BPath path(&nref);
	BString filename(path.Path());
	
	//do it
	LOG("MainController",liDebug,"Unlinking %s",filename.String());

	ActionDelete del(filename,true);
	status_t ret = del.Perform(NULL);
		
	if(ret !=B_OK ){
		LOG("MainController",liDebug,"Error %s",strerror(errno));
		return B_ERROR;
	}
	
	RemoveChannelFromUI(nref,row);
	return B_OK;
}
				
void		
MainController::RemoveChannelFromUI(entry_ref nref,SubscriptionListItem* row){
	fModel->fChInfo.RemoveItemFor(nref);
	fView->RemoveSubscription(row);
//	fView->RefreshSubscriptionLists(); //maybe not needed but it's ok..
}

void	
MainController::CheckChannel(entry_ref nref){

	SubscriptionListItem* 	row = fModel->fChInfo.ValueFor(nref);
	if(!row) return;
	
	if( row->GetFileStatus()==ENQUEQUED ||
	    row->GetFileStatus()==CONNECTING) return;
	
	BDirectory dir(&row->fRef);
	
	BPath apath(&dir,NULL);
	apath.Append("rss.xml");

	entry_ref ref;
	get_ref_for_path(apath.Path(),&ref);
	
		BString url(row->GetURL());	
		/*
			check channel url.
			replace pcast:// and feed://
		*/
		if(url.FindFirst("pcast://") == 0)
			url.ReplaceFirst("pcast://","http://");
		else
		if(url.FindFirst("feed://") == 0)
			url.ReplaceFirst("feed://","http://");
			
		
	ActionDownload* ac= new ActionDownload(url,ref,false, "");
	ac->SetActionID(real_time_clock_usecs());
	ac->SetRef("channel_ref",&row->fRef);
		
	//let's go..
	row->SetFileStatus(ENQUEQUED);
	download_manager->Enqueue(DownloadManager::CHANNELS_QUEUE,ac);
	
	fView->RefreshSubscription(row);
		
}
	
void	
MainController::CheckAllChannel(){

	//the fView should be locked!
	if(!fView->IsLocked()) return;
	
	int32 count =fModel->fChInfo.CountItems();
	
	for(int i=0;i<count;i++){
			SubscriptionListItem* 	row = fModel->fChInfo.ValueAt(i);
			if(row) 
				CheckChannel(row->fRef);
	}
	

}	

void
MainController::InfoExtraDownload(BMessage* msg){

			entry_ref nref;
			if( msg->FindRef("channel_ref",&nref) != B_OK) return;
					
			SubscriptionListItem* 	row = fModel->fChInfo.ValueFor(nref);
			if(!row) return;
			
			
			RssChannel* channel=&row->fChannel;
			if(!channel) return;
			
			ActionDownload::Status status = (ActionDownload::Status)msg->FindInt32("status");
			switch(status){
				
				case ActionDownload::OK_DOWNLOADED:
				{
					entry_ref ref;
					BString extra;
					
					msg->FindRef("entry",&ref);
					msg->FindString("extra",&extra);
					
					if(extra.ICompare("image") == 0)
					{
						
						BBitmap	*logo = BTranslationUtils::GetBitmap(&ref);
						if(!logo) return;

						// Scale To Icon size
						// TODO: if the size of the subscription icon can change this must
						// be rescaled from the full image
						// Icon Size is 46 set in SubscriptionListItem
						BBitmap *scalingBitmap = new BBitmap(BRect(0,0,45,45), B_RGBA32, true);
						if (scalingBitmap != NULL) {
								BView *scalingView = new BView(logo->Bounds(),
									"ScalingView", B_FOLLOW_NONE, B_WILL_DRAW);
								scalingBitmap->AddChild(scalingView);
								scalingBitmap->Lock();
								//scalingView->DrawBitmap(logo, logo->Bounds(), scalingBitmap->Bounds(), B_FILTER_BITMAP_BILINEAR);
								scalingView->DrawBitmap(logo, logo->Bounds(), scalingBitmap->Bounds());
								scalingBitmap->Unlock();
								scalingBitmap->RemoveChild(scalingView);
								delete scalingView;
						} else {
							delete logo;
							return;
						}
						// what to do with this image?
						AttributeDumper dump(&row->fRef);
						row->fChannel.AddElementListener(&dump);
						row->fChannel.SetKey(CHANNEL_IMAGE_DATA,(void*)scalingBitmap,sizeof(BBitmap*));
						row->fChannel.RemoveElementListener(&dump);

						delete scalingBitmap;
						delete logo;
					}
					
					fView->RefreshSubscription(row);
				}
				default:break;
			}
}


void 
MainController::StoreStates(){

	int32 count = fModel->fChInfo.CountItems();
	
	BMessage cache;
	BDirectory dir(&fModel->archive_ref);
	
	for(int i=0;i<count;i++){
	
		SubscriptionListItem* row = fModel->fChInfo.ValueAt(i);
		if(!row) continue;
		
		BDirectory node(&row->fRef);
		if(node.InitCheck()!=B_OK) continue;
				
		//do: write attribute!
		
		if(row->columnsState)			
			WriteAttributeMessage(&node,ATTR_COLUMNS_STATE,row->columnsState);			

	 	
	 	//now I can create the extra 'state'
	 	BMessage extra;
	 	SaveExtraState(&extra,row);
		WriteAttributeMessage(&node,ATTR_EXTRA_STATE,&extra);
		
		//caching modification time...
		time_t when;
		if(node.GetModificationTime(&when)==B_OK) {
			BString key=row->GetURL();
			cache.AddInt64(key.String(),(int64)when);
			key << "_count";
			cache.AddInt32(key.String(),row->GetNewCount());
		}
	}
	
	WriteAttributeMessage(&dir,ATTR_CACHE,&cache);
	
	
	//STORE GROUPS:
	BMessage groups;
	SaveGroupSetting(&groups);
	WriteAttributeMessage(&dir,ATTR_GROUPS,&groups);
}

void 
MainController::QuitRequested() {
	
	BList removed(5);
	download_manager->RemoveQueue(DownloadManager::ITEMS_QUEUE,&removed);
	
	for(int i=0; i< removed.CountItems(); i++){

		ActionDownload* down = (ActionDownload*) ( removed.ItemAt(i) );
		
		entry_ref ref;
		if(down->GetRef("entry_ref",&ref)!=B_OK) continue;

		BPEpisode bpe(&ref);
		bpe.SetStatus(STOPPED);
		
		//FIX: forse bisogna scollegare l'item grafico? bho ma chi se ne frega.. stiamo quittando..
	}
	
	removed.MakeEmpty();

	StoreStates();
}

void	
MainController::AddNewGroup(const char* label){
	
	GroupItem* gi=NULL;
	BString text(label);
	
	int extra=0;
				
	do
	{
		text.SetTo(label);
					
		if(label==NULL)
			text << _T("Group") << " " << fModel->fGroups.CountItems()+(extra++);
		else
			if(extra>0)
				text << " " << extra;
			
		gi=fModel->fGroups.ValueFor(text);
		extra++;

	} while(gi);
	
	//we have the name
	
	GroupItem* group=new GroupItem(text.String(), NULL);
	fModel->fGroups.AddItem(text,group);
	fView->AddGroup(group,true);
}

void	
MainController::RemoveGroup(BString name) {
	GroupItem* new_g=fModel->fGroups.ValueFor(name);
	if(!new_g) return;
	
	fView->RemoveGroup(new_g);
	fModel->fGroups.RemoveItemFor(name);
}

void	
MainController::RenameGroup(BString oldname,BString newname) {
	GroupItem* new_g=fModel->fGroups.ValueFor(newname);
	if(new_g) return;
		
	GroupItem* old_g=fModel->fGroups.ValueFor(oldname);
	if(!old_g) return;
	
	old_g->SetGroupName(newname);

	fModel->fGroups.RemoveItemFor(oldname);
	fModel->fGroups.AddItem(newname,old_g);
}


GroupItem*
MainController::AddSavedGroup(BString text,bool expanded){
	
	if(text == "") return NULL;
	
	GroupItem* gi=fModel->fGroups.ValueFor(text);
	if(gi) return gi; //already present.
	
	GroupItem* group = new GroupItem(text.String(), NULL);
	fModel->fGroups.AddItem(text,group);
	
	if(fView->Lock()){	
		fView->AddGroup(group,expanded);
		fView->Unlock();
	}
	
	return group;	
}

void	
MainController::UnlinkDownloadItems(){
	int32 count=fModel->fDownloadList.CountItems();
	for(int i=0;i<count;i++)
		fModel->fDownloadList.ValueAt(i)->Unlink();
}

void				
MainController::SwitchChannel(entry_ref* href) {

	//MainWindow DEVE essere Locked.
	if(!fView->IsLocked()) return;
	
	SubscriptionListItem* 	row = NULL;
	
	if(href){
		row = fModel->fChInfo.ValueFor(*href);
		if( BDirectory(&row->fRef) == fModel->fSelectedChannel) return;
	}
	
	
	if(fModel->fSelectedChannel.InitCheck()==B_OK){
		
		//giro del cazzo:
		BEntry e;
		entry_ref oldref;
		fModel->fSelectedChannel.GetEntry(&e);
		e.GetRef(&oldref);
		
		SubscriptionListItem* oldRow = fModel->fChInfo.ValueFor(oldref);
		if(oldRow){
				//------> Save info on the ROW!..
				oldRow->SetIsNew(false);
				if(!oldRow->columnsState)
					oldRow->columnsState=new BMessage();
				
				oldRow->columnsState->MakeEmpty();
				SaveColumnsState(oldRow->columnsState);
				//FIX: manca salvataggio ultimo episode selezionato e anche scrollposition verticale!
				//oldRow->lastSelected = fModel->fSelectedItem;
		}

	}
	
	
	UnlinkDownloadItems();
	fView->ClearEpisodeListView();
			
//	RssChannel* channel=&row->fChannel;
//	if(!channel) return;


	//recall saved columns infos
	if(row){
		
	if(row->columnsState)
		LoadColumnsState(row->columnsState);
		
		fValidThreads.clear();
		SetSelectedItem(NULL,&row->fRef);
		ParseChannel(row->fRef);
	} else {
	
		fValidThreads.clear();
		SetSelectedItem(NULL,NULL);	
	}
	
		
}

void				
MainController::ParseChannel(entry_ref ref) {
	//crea thread, 
	// e via si parte.
	
	ParseParams *params = new ParseParams;
	params->nref = ref;
	params->fController = this;


	thread_id addPosesThread = spawn_thread(&MainController::ParsingThread, "parser",B_DISPLAY_PRIORITY, params);

	if (addPosesThread >= B_OK) {
		fValidThreads.insert(addPosesThread); 
		resume_thread(addPosesThread);
	} 
	else
		delete params;
		
	
}

int32	
MainController::ParsingThread(void* data) {
	//codice del thread. se
	ParseParams *params = (ParseParams*)data;	
	MainController *fController=params->fController;
	entry_ref	ref = params->nref;
	
	
	thread_id threadID = find_thread(NULL);
	
	BDirectory dir(&ref);
	BEntry		entry;

	while(dir.GetNextEntry(&entry) == B_OK && fController->fView->Lock()  ) {
		
		if(fController->IsValidThread(threadID)){		
				fController->AddEpisode(entry);
			}
			
			fController->fView->Unlock();
	}	
	
	delete params;
	fController->fValidThreads.erase(threadID); 
	return 0;
}
				

bool
MainController::IsValidThread(thread_id currentThread) const {
	return fValidThreads.find(currentThread) != fValidThreads.end();
}

void
MainController::AddEpisode(BEntry entry) {
	
		AttributeExtractor an;
			
		BNode	item(&entry);
		if(entry.InitCheck()==B_OK && !entry.IsDirectory() && CheckMIME(MIME_ITEM,&item) ) {
			
			RssItem fake;
			
			EpisodeListItem *ep=new EpisodeListItem();
			entry.GetRef(&ep->fRef);
			fake.AddElementListener(ep);
			
			an.SetNotifier(&fake);
			an.Parse(&item);
			
			
			
			//if the Rss doesn't have a UID?
			if(fake.Guid() == "") {
				delete ep;
				return;
			}
			
			DownloadListItem* dwn=fModel->fDownloadList.ValueFor(ep->fRef);
			if(dwn)
			{
					dwn->LinkToEpisode(ep);
					dwn->fRss.SetFileStatus(dwn->fRss.GetFileStatus());
					//TODO: altre info?
					dwn->fRss.SetDownloadSpeed(dwn->GetLastSpeed());
					dwn->fRss.SetFilePercentage(dwn->GetPercentage());
					
			}
			else
			{
					
					// check for percentage.
					off_t total=ep->GetEnclosureSize();
					if(fake.GetFileStatus() == STOPPED && total>0){
						BPath path(&entry);
						path.GetParent(&path);
						path.Append("enclosure");
						path.Append(fake.EnclosureLocalPath().String());
						off_t size = 0;
						entry.SetTo(path.Path());
						if(entry.InitCheck()==B_OK && entry.GetSize(&size)==B_OK && total>size ){
							fake.SetFilePercentage((int32)((size * 100/ total )));	
						}
					}
					// end check for percentage.
			}
			
			fView->AddEpisodeListItem(ep);
			fake.RemoveElementListener(ep); //safety.
	}
}
