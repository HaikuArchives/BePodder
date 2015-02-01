#include "TheApp.h"
#include "FilePanel.h"

#include <Message.h>
#include <stdio.h>

#include "owner.h"
#include "Logger.h"

#include <Application.h>
#include <Roster.h>
#include <Entry.h>
#include <Message.h>
#include "BPLocale.h"


#include "BPAlert.h"
#include "Utils.h"
#include "RVAction.h"

#include "Setting.h"
#include "BPSettingsWindow.h"
//#include "LicenceInfo.h"
#include "MainModel.h"
#include "HelpViewer.h"
#include	"AttributeMap.h"
#include "InterfaceDefs.h"


#include "IActionManagerBP.h"
#include "ActionHTTPContent.h"
#include <MimeType.h>
#include "DownloadManager.h"

extern DownloadManager*	download_manager;


#define	UPDATE_INFO		'updt' 
#define	CHECK_UPDATE		'ckup'

MainWindow*				main_window;
IActionManagerBP		action_manager;
Setting 				podder_settings("BePodder.conf");
//LicenceInfo				linfo("BePodder.key");


TheApp::TheApp(const char * signature) :
	BApplication(signature)
{
	//load settings from disk:
	podder_settings.Retrive();

	InstallMimeTypes();
		
	MainModel *fModel = new MainModel();
	main_window=new MainWindow();
	fController = new MainController(main_window,fModel);
	
	action_manager.init(fController,main_window);

	main_window->init(fController);
}

void
TheApp::ReadyToRun(){

//	linfo.LoadFromFile();
	
	ApplySettings();
	ApplyWindowSettings();
	
	main_window->Show();
	snooze(200000);
	
//	BMessage grp;
//	if( podder_settings.FindMessage(SETTINGS_GROUPS,&grp) == B_OK)
//		if(main_window->Lock()){
//			fController->LoadGroupSetting(&grp);
//			main_window->Unlock();
//		}
	
	
	this->PostMessage(ARCHIVE_PARSE);



// we disabledupdates?

	BMessage msg;
	bool val = true;
	if( podder_settings.FindMessage(SETTINGS_UPDATES,&msg) == B_OK) {
		if(msg.FindBool("check_updates",&val) != B_OK) val= true;
	}
	
	if(val)
		ChekUpdate();
		



	return;
}

void		
TheApp::ChekUpdate() {
	
	/************
	
		CHECKING UPDATE IS DISABLED.
	
	********/
	
	return;
	
	//uhm check for update?
	printf("Checking for updates..\n");
	ActionHTTPContent*		update=new ActionHTTPContent("http://www.funkyideasoft.com/latest.nfo");	
	update->SetTargetAndWhat(this,UPDATE_INFO);
	download_manager->SingleThreadAction(update);
}

void
TheApp::AboutRequested(){
	
	BString text("\nBePodder");
	text <<"\n\nDevelopers:\n\tAndrea Anzani [xeD]\n\tGiuseppe Gargaro [Beck]\n\nGraphic:\n\tzuMi\n\n\n";
	text << "Version : " << VERSION_NAME << " " << VERSION_ID << "\n";
		
	BPAlert* about = new BPAlert("about", text.String() ,_T("Close!"),NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("logo-64.png"));
	about->Go();
}

bool
TheApp::QuitRequested(){

	
	if(main_window->Lock()){
		
		fController->QuitRequested();	
		
		BMessage data;
		main_window->SaveSetting(&data);
		podder_settings.RemoveData(SETTINGS_MAINWINDOW);
		podder_settings.AddMessage(SETTINGS_MAINWINDOW,&data);
	
		main_window->Quit();
	}
	podder_settings.Store();
	return true;
}

void 
TheApp::ArgvReceived(int32 argc, char **argv){
			
		//	debugger("ARGV");
			
			if(argc<2) return;
			
			main_window->Activate();
			
			BMessage	msg('addu');
			msg.AddString("url",argv[1]);
			BMessenger(main_window).SendMessage(&msg);
}


void
TheApp::MessageReceived(BMessage* msg){

	//B_ARGV_RECEIVED 
	
	switch(msg->what){

	case CHECK_UPDATE:
		ChekUpdate();
	break;
	case UPDATE_INFO:
	{
		BString rel;
		if(msg->FindString("content",&rel)==B_OK && rel != "" && rel.Compare(VERSION_ID) != 0)
		{
			//FIX da tradurre!
			BString text("A new release of this application is available!\n\n Your version : ");
			text << VERSION_ID << "\n\n" << "Latest available version : " << rel ;
			text << "\n\nGo to our web site and download the latest version!\n" ;
						
			BPAlert *alert = new BPAlert("BePodder",text.String(),_T("Web Site"),_T("Close"),NULL,B_WIDTH_AS_USUAL, LoadIcon("logo-64.png"));
			if(alert->Go()==0)
				fController->OpenURL("http://www.funkyideasoft.com/downloads.html");
		}
	}
	break;
	case PERFORM_ACTION:
		action_manager.Perform(msg);
	break;
	case ARCHIVE_PARSE:
	{
			//show the alert
			BPAlert *alert = new BPAlert("BePodder",_TT("alert3"),NULL,NULL,NULL,B_WIDTH_AS_USUAL, LoadIcon("enqueued-32.png"));
			alert->Go(NULL);
			

			fController->ParseArchive(); 
					
			main_window->SelectSubscription(0);
			//close the alert
			alert->PostMessage(B_QUIT_REQUESTED);
			
	}
	break;
	case APPLY_SETTINGS:
		ApplySettings();
	break;
	case 'lic2':
	{
		
		BPAlert* about = new BPAlert("Sorry", _T("Shareware application!\n\nPlease support us!\n\nBuy a software licence!\nMore info on our website\n"),_T("Web Site"),_T("Close"),NULL,B_WIDTH_AS_USUAL,LoadIcon("logo-64.png"));
		if(about->Go()==0){
				fController->OpenURL("http://www.funkyideasoft.com/bepodder.html");
		}
				
	}
	break;
		case HELPVIEWER_OPEN_URL:
	{
			BString url;
			if(msg->FindString("url",&url)==B_OK){
				if(fController)
					fController->OpenURL(url);
			}
	}	
	break;
	default:
		BApplication::MessageReceived(msg);
	break;
	}
}

void
TheApp::ApplySettings(){

	BMessage msg;

	msg.MakeEmpty();	
	if( podder_settings.FindMessage(SETTINGS_DOWNLOAD,&msg) == B_OK)
		fController->LoadSettings(SETTINGS_DOWNLOAD,&msg);	
		
	msg.MakeEmpty();	
	if( podder_settings.FindMessage(SETTINGS_PROXY,&msg) == B_OK)
		fController->LoadSettings(SETTINGS_PROXY,&msg);	
		
		
	msg.MakeEmpty();	
	if( podder_settings.FindMessage(SETTINGS_FILETYPE,&msg) == B_OK)
			fController->LoadSettings(SETTINGS_FILETYPE,&msg);		
	
	
	msg.MakeEmpty();	
	if( podder_settings.FindMessage(SETTINGS_CHANNELS,&msg) == B_OK)
		fController->LoadSettings(SETTINGS_CHANNELS,&msg);		
}

void
TheApp::ApplyWindowSettings(){

	BMessage msg;
	if( podder_settings.FindMessage(SETTINGS_MAINWINDOW,&msg) == B_OK)
		if(main_window->Lock()){
			main_window->LoadSetting(&msg);
			main_window->Unlock();
		}
}


void
TheApp::InstallMimeTypes(){

	BMimeType	channel(MIME_CHANNEL);
	
	if (channel.IsInstalled()) {
		BMessage info;
		BString revision;
		if( channel.GetAttrInfo(&info) == B_OK && info.FindString("revision",&revision)==B_OK && revision.ICompare("1.2")==0)	
			return;
			
			
		channel.Delete();
	}

	printf("Installing Channel MimeType..\n");
	
	channel.SetShortDescription("BePodder Channel");
	
	BMessage achannel;
	
	achannel.AddString("attr:name","bp:title");
	achannel.AddString("attr:public_name","Title");
	achannel.AddInt32("attr:type",B_STRING_TYPE);
	achannel.AddInt32("attr:width",30);
	achannel.AddInt32("attr:alignment",B_ALIGN_LEFT);
	achannel.AddBool("attr:extra",false);
	achannel.AddBool("attr:viewable",true);
	achannel.AddBool("attr:editable",false);
		
	achannel.AddString("attr:name","bp:link");
	achannel.AddString("attr:public_name","Feed Link");
	achannel.AddInt32("attr:type",B_STRING_TYPE);
	achannel.AddInt32("attr:width",30);
	achannel.AddInt32("attr:alignment",B_ALIGN_LEFT);
	achannel.AddBool("attr:extra",false);
	achannel.AddBool("attr:viewable",true);
	achannel.AddBool("attr:editable",false);
		
	achannel.AddString("attr:name","bp:category");
	achannel.AddString("attr:public_name","Category");
	achannel.AddInt32("attr:type",B_STRING_TYPE);
	achannel.AddInt32("attr:width",30);
	achannel.AddInt32("attr:alignment",B_ALIGN_LEFT);
	achannel.AddBool("attr:extra",false);
	achannel.AddBool("attr:viewable",true);
	achannel.AddBool("attr:editable",false);
		
	achannel.AddString("attr:name","bp:weblink");
	achannel.AddString("attr:public_name","Website");
	achannel.AddInt32("attr:type",B_STRING_TYPE);
	achannel.AddInt32("attr:width",30);
	achannel.AddInt32("attr:alignment",B_ALIGN_LEFT);
	achannel.AddBool("attr:extra",false);
	achannel.AddBool("attr:viewable",true);
	achannel.AddBool("attr:editable",false);
	
//	achannel.AddString("revision","1.2");
	
	channel.SetAttrInfo(&achannel);
	
	if(channel.Install()!=B_OK)
		printf("Error Installing!\n");	
	
	
	BMimeType	item(MIME_ITEM);
	
	if (item.IsInstalled()) {
		BMessage info;
		BString revision;
		if( item.GetAttrInfo(&info) == B_OK && info.FindString("revision",&revision)==B_OK && revision.ICompare("1.2")==0)	
			return;
			
		item.Delete();
	}

	printf("Installing Item MimeType..\n");
	
	item.SetShortDescription("BePodder Episode");
	
	
	BMessage aitem;
		
	aitem.AddString("attr:name","bp:item_title");
	aitem.AddString("attr:public_name","Title");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);
	
	aitem.AddString("attr:name","bp:item_pubdate");
	aitem.AddString("attr:public_name","Published");
	aitem.AddInt32("attr:type",B_TIME_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);
	
	aitem.AddString("attr:name","bp:item_guid");
	aitem.AddString("attr:public_name","Guid");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);
	
	aitem.AddString("attr:name","bp:enclosure_size");
	aitem.AddString("attr:public_name","Enclosure Size");
	aitem.AddInt32("attr:type",B_OFF_T_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_RIGHT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);

	aitem.AddString("attr:name","bp:item_status");
	aitem.AddString("attr:public_name","Status");
	aitem.AddInt32("attr:type",B_INT32_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_CENTER);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);		

	aitem.AddString("attr:name","bp:enclosure_url");
	aitem.AddString("attr:public_name","Enclosure URL");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);
	
	aitem.AddString("attr:name","bp:enclosure_path");
	aitem.AddString("attr:public_name","Enclosure Path");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);

	aitem.AddString("attr:name","bp:item_link");
	aitem.AddString("attr:public_name","Link");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);	

	aitem.AddString("attr:name","bp:category");
	aitem.AddString("attr:public_name","Category");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);	
	
	aitem.AddString("attr:name","bp:item_keywords");
	aitem.AddString("attr:public_name","Keywords");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);	
	
	aitem.AddString("attr:name","bp:enclosure_type");
	aitem.AddString("attr:public_name","Enclosure Type");
	aitem.AddInt32("attr:type",B_STRING_TYPE);
	aitem.AddInt32("attr:width",30);
	aitem.AddInt32("attr:alignment",B_ALIGN_LEFT);
	aitem.AddBool("attr:extra",false);
	aitem.AddBool("attr:viewable",true);
	aitem.AddBool("attr:editable",false);
	
//	aitem.AddString("revision","1.2");			
			
	item.SetAttrInfo(&aitem);
	if(item.Install()!=B_OK)
		printf("Error Installing!\n");	
		
	BMimeType	ch(MIME_ARCHIVED_CHANNEL);
	
	if (ch.IsInstalled()) {
		BMessage info;
		BString revision;
		if( ch.GetAttrInfo(&info) == B_OK && info.FindString("revision",&revision)==B_OK && revision.ICompare("1.2")==0)	
			return;			
		ch.Delete();
	}

	printf("Installing Archived Channel MimeType..\n");
	
	ch.SetShortDescription("BePodder Archived Channel");
	
	BMessage ach;
//	ach.AddString("revision","1.2");			
			
	ch.SetAttrInfo(&ach);
	if(ch.Install()!=B_OK)
		printf("Error Installing!\n");	
}
