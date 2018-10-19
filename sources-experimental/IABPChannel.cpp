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
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("add-channel-file", 24));
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
		*modifiers = B_COMMAND_KEY;
	return 'N';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelRemove::IABPChannelRemove(MainController* controller ,MainWindow* view):IActionBP(controller,view){
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("delete-channel-file", 24));
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
				BBitmap *bitmap=new BBitmap( ((SubscriptionField*)row->GetField(0))->GetBitmap() );
				
				BString text;
		
				text << B_TRANSLATE("You are going to remove the channel:\n\n");
				text << row->GetTitle();
				text << B_TRANSLATE("\n\nWould you like just to archive this channel (keeping all the files and enclosures downloaded so far) or do you want to DELETE the whole channel from your computer?\n");

				BPAlert* remove = new BPAlert(B_TRANSLATE("Remove a channel"), text.String(),B_TRANSLATE("Delete"),B_TRANSLATE("Archive"),B_TRANSLATE("Cancel"),B_WIDTH_AS_USUAL,bitmap);
				int32 result=remove->Go();
				
				LOCKWINDOW
				if(result==1)
					fController->ArchiveChannel(row->fRef);
				else
				if(result==0){
					BPAlert* wait = new BPAlert(B_TRANSLATE("Remove a channel"),
						B_TRANSLATE("\nRemoving" B_UTF8_ELLIPSIS),NULL,NULL,NULL,
						B_WIDTH_AS_USUAL,bitmap);
					fView->UpdateIfNeeded();
					wait->Go(NULL);
					
					status_t result = fController->DeleteChannel(row->fRef);
					
					wait->PostMessage(B_QUIT_REQUESTED);
					
					if(result !=B_OK){
						wait = new BPAlert(B_TRANSLATE("Remove a channel"),
						B_TRANSLATE("\nAn error occured while removing!"),
						B_TRANSLATE("OK"),NULL,NULL,B_WIDTH_AS_USUAL,
						bitmap);
						wait->Go();
					}
				}
				UNLOCKWINDOW
				delete bitmap;
	return B_OK;
} 

char 
IABPChannelRemove::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return B_DELETE;
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelCheck::IABPChannelCheck(MainController* controller,MainWindow* view):IActionBP(controller,view){
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("refresh-channel-file", 24));
				SetLabel(B_TRANSLATE("Check channel"));
}
			 	 
BString 
IABPChannelCheck::GetDescription(){
	return B_TRANSLATE("Check the selected podcast for new episodes");
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
		*modifiers = B_COMMAND_KEY;
	return 'R';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelWWW::IABPChannelWWW(MainController* controller,MainWindow* view):IActionBP(controller,view){
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("www-channel-file", 24));
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
	 	  		SetLabel(B_TRANSLATE("Check all channels")); 
}
			 	 
BString 
IABPChannelCheckAll::GetDescription(){
	return B_TRANSLATE("Check all the podcasts for new episodes");
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
	return 'R';
}

//-------------------------------------------------------------------------------------------------------------------

IABPChannelShowImage::IABPChannelShowImage(MainController* controller,MainWindow* view):IActionBP(controller,view){
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
