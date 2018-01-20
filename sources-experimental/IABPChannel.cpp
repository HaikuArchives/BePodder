/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

//actions for channels

//#include "SubscriptionListItem.h"
#include "IABPChannel.h"
#include "MainWindow.h"
#include "MainController.h"
#include <Catalog.h>
#include <Roster.h>
#include "ChannelSettingsWindow.h"

//extern BMessage podder_settings;

#define		LOCKWINDOW					if(fView->Lock()){	
#define		UNLOCKWINDOWERROR	{ fView->Unlock(); return B_ERROR; }
#define		UNLOCKWINDOW				fView->Unlock();}

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "IABPChannel"

//-------------------------------------------------------------------------------------------------------------------


IABPChannelAddRequest::IABPChannelAddRequest(MainController* controller, MainWindow* view ):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("emblem-add.png"));
	 	   		SetIcon(IAction::SIZE_48,LoadIcon("add-channel-file.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("add-channel-file-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Add channel")); 
			 	  
}
			 	 
BString 
IABPChannelAddRequest::GetDescription(){
 	  return B_TRANSLATE("Add the podcast to the subscriptions list");
}
			 	  

status_t 
IABPChannelAddRequest::Perform(BMessage*){
  		AddWindow *url = new AddWindow();
		url->Show();
		return B_OK;
} 

char 
IABPChannelAddRequest::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
	return 'A';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelRemove::IABPChannelRemove(MainController* controller ,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("delete-micro.png"));
	 	   		SetIcon(IAction::SIZE_48,LoadIcon("delete-channel-file.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("delete-channel-file-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Remove channel")); 
			 	  
}
			 	 
BString 
IABPChannelRemove::GetDescription(){
 	  return B_TRANSLATE("Delete the podcast from the subscriptions list");
}
			 	  

status_t 
IABPChannelRemove::Perform(BMessage*){
	

			
			SubscriptionListItem* row=fView->GetSelectedSubscription();
			if(!row) return B_ERROR;
		
				
				//TODO: studiare la disposizione dei pulsanti!
				//TODO FIX THIS LOCALE
				
				BString text;
		
				text << B_TRANSLATE("\nYou are going to remove the channel:\n\n");	
				text << row->GetTitle();
				text << B_TRANSLATE("\n\nWould you like just to archive this channel (keeping all the files and enclosures downloaded so far) or do you want to DELETE the whole channel from you computer?");
							
				BPAlert* remove = new BPAlert("Remove a channel", text.String(),B_TRANSLATE("Delete"),B_TRANSLATE("Archive"),B_TRANSLATE("Cancel"),B_WIDTH_AS_USUAL,LoadIcon("delete-channel-file.png"));
				int32 result=remove->Go();
				
				LOCKWINDOW
				if(result==1)
					fController->ArchiveChannel(row->fRef);
				else
				if(result==0){
					BPAlert* wait = new BPAlert("Remove a channel", "\nRemoving..",NULL,NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("delete-channel-file.png"));
					fView->UpdateIfNeeded();
					wait->Go(NULL);
					
					status_t result = fController->DeleteChannel(row->fRef);
					
					wait->PostMessage(B_QUIT_REQUESTED);
					
					if(result !=B_OK){
						wait = new BPAlert("Remove a channel", "\nAn error occured while removing!",B_TRANSLATE("Ok"),NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("delete-channel-file.png"));
						
						wait->Go();
					}
					
					
				}
				UNLOCKWINDOW

	return B_OK;
} 

char 
IABPChannelRemove::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
	return 'R';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelCheck::IABPChannelCheck(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("refresh-micro.png"));
	 	   		SetIcon(IAction::SIZE_48,LoadIcon("refresh-channel-file.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("refresh-channel-file-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Check channel")); 
			 	  
}
			 	 
BString 
IABPChannelCheck::GetDescription(){
 	  return B_TRANSLATE("Check the selected podcast for new items");
}
			 	  

status_t 
IABPChannelCheck::Perform(BMessage*){
			LOCKWINDOW
			SubscriptionListItem* row=fView->GetSelectedSubscription();
			if(!row) UNLOCKWINDOWERROR
			
			fController->CheckChannel(row->fRef);
			UNLOCKWINDOW
			return B_OK;
}

char 
IABPChannelCheck::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
	return 'C';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelWWW::IABPChannelWWW(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("www-micro.png"));
	 	   		SetIcon(IAction::SIZE_48,LoadIcon("www-channel-file.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("www-channel-file-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Open homepage")); 
			 	  
}
			 	 
BString 
IABPChannelWWW::GetDescription(){
 	  return B_TRANSLATE("Show the podcast homepage");
}
			 	  

status_t 
IABPChannelWWW::Perform(BMessage*){
		
				SubscriptionListItem* row=fView->GetSelectedSubscription();
				if(!row || row->GetWeblink()=="" ) return B_ERROR;
				
				fController->OpenURL(row->GetWeblink());
				return B_OK;
}

char 
IABPChannelWWW::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
	return 'H';
}

//-------------------------------------------------------------------------------------------------------------------


IABPChannelEnclosureFolder::IABPChannelEnclosureFolder(MainController* controller,MainWindow* view):IActionBP(controller,view){
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("folder"));
 		
	 	  		SetLabel(B_TRANSLATE("Open enclosures folder")); 
			 	  
}
			 	 
BString 
IABPChannelEnclosureFolder::GetDescription(){
 	  return B_TRANSLATE("Open enclosures folder");
}
			 	  

status_t 
IABPChannelEnclosureFolder::Perform(BMessage*){
			LOCKWINDOW
			SubscriptionListItem* row=fView->GetSelectedSubscription();
			if(row)
				fController->FolderSelectedChannel(row->fRef);
			UNLOCKWINDOW
			return B_OK;
}

char 
IABPChannelEnclosureFolder::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY ;
	return 'O';
}


//-------------------------------------------------------------------------------------------------------------------

IABPChannelCheckAll::IABPChannelCheckAll(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("refresh-mini-all.png"));
	 	   		//SetIcon(IAction::SIZE_48,LoadIcon("refresh-channel-file.png"));
	 	  		//SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("refresh-channel-file-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Check all channels")); 
			 	  
}
			 	 
BString 
IABPChannelCheckAll::GetDescription(){
 	  return B_TRANSLATE("Check all the podcasts for new items");
}
			 	  

status_t 
IABPChannelCheckAll::Perform(BMessage*){
			fView->PostMessage(CHECK_ALL);
			return B_OK;
}

char 
IABPChannelCheckAll::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
	return 'N';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelShowImage::IABPChannelShowImage(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetIcon(IAction::SIZE_16,LoadIcon("image"));
	//SetIcon(IAction::SIZE_48,LoadIcon("refresh-channel-file.png"));
	//SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("refresh-channel-file-down.png"));
	SetLabel(B_TRANSLATE("Show channel image")); 
}
			 	 
BString 
IABPChannelShowImage::GetDescription(){
 	  return B_TRANSLATE("Show channel image");
}
			 	  

status_t 
IABPChannelShowImage::Perform(BMessage*){
	LOCKWINDOW
	SubscriptionListItem* row=fView->GetSelectedSubscription();
	if(row)
		fController->ShowChannelImage(row->fRef);
	//if(!row->extraSetting) row->extraSetting=new BMessage();
	//ChannelSettingsWindow* csw=new ChannelSettingsWindow(row->extraSetting,fView);
	//csw->Show();
	
	UNLOCKWINDOW
	return B_OK;
}

char 
IABPChannelShowImage::Shortcut(uint32 *modifiers) const {
	return 0;
}
