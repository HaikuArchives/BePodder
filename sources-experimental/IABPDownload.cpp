/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

//actions for items
#include <Catalog.h>
#include "IABPDownload.h"
#include "MainWindow.h"
#include "MainController.h"

#define		LOCKWINDOW					if(fView->Lock()){	
#define		UNLOCKWINDOWERROR	{ fView->Unlock(); return B_ERROR; }
#define		UNLOCKWINDOW				fView->Unlock();}


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "IABPDownload"

IABPDownloadRemove::IABPDownloadRemove(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("Remove from this list"));
}
			 	 
BString 
IABPDownloadRemove::GetDescription(){
	return ("");
}
			 	  

status_t 
IABPDownloadRemove::Perform(BMessage*){

		LOCKWINDOW
		DownloadListItem* row=fView->GetSelectedDownload();
			if(row)
				fController->RemoveDownload(row->fRef);
		UNLOCKWINDOW
		return B_OK;
} 

char 
IABPDownloadRemove::Shortcut(uint32 *modifiers) const {
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------


IABPDownloadStop::IABPDownloadStop(MainController* controller, MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("Stop download"));
}
			 	 
BString 
IABPDownloadStop::GetDescription(){
	return B_TRANSLATE("Stop the episode download");
}
			 	  

status_t 
IABPDownloadStop::Perform(BMessage*){
		
		LOCKWINDOW
			DownloadListItem* row=fView->GetSelectedDownload();
			if(row)
				fController->StopDownloadItem(row->fRef);
		UNLOCKWINDOW	
		
		return B_OK;
} 

char 
IABPDownloadStop::Shortcut(uint32 *modifiers) const {
	/*if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return 'S';
	*/
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------


IABPDownloadDownload::IABPDownloadDownload(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("(Re)start download"));
}
			 	 
BString 
IABPDownloadDownload::GetDescription(){
	return B_TRANSLATE("Download the selected episode");
}
			 	  

status_t 
IABPDownloadDownload::Perform(BMessage*){
		LOCKWINDOW
			DownloadListItem* row=fView->GetSelectedDownload();
			if(row)
				fController->DownloadItem(row->fRef);
		UNLOCKWINDOW	
		return B_OK;
} 


char 
IABPDownloadDownload::Shortcut(uint32 *modifiers) const {
	/*if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return 'D';*/
	return (char)NULL;
}


//-------------------------------------------------------------------------------------------------------------------


IABPDownloadPlay::IABPDownloadPlay(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("Play enclosure"));
}
			 	 
BString 
IABPDownloadPlay::GetDescription(){
	return B_TRANSLATE("Play the selected episode");
}
			 	  

status_t 
IABPDownloadPlay::Perform(BMessage*){
		LOCKWINDOW
			DownloadListItem* row=fView->GetSelectedDownload();
			if(row)
				fController->PlayItem(row->fRef);
		UNLOCKWINDOW	
		
		return B_OK;
} 

char 
IABPDownloadPlay::Shortcut(uint32 *modifiers) const {
	/*if(modifiers)
		*modifiers = B_COMMAND_KEY;	
	return 'E';
	*/
	
	return (char)NULL;
}


//-------------------------------------------------------------------------------------------------------------------

IABPDownloadFindItem::IABPDownloadFindItem(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("Find this episode"));
}
			 	 
BString 
IABPDownloadFindItem::GetDescription(){
	return ("");
}
			 	  

status_t 
IABPDownloadFindItem::Perform(BMessage*){
		LOCKWINDOW
		DownloadListItem* row=fView->GetSelectedDownload();
			if(row)
				fController->FindItem(row->fRef);
		UNLOCKWINDOW	
		return B_OK;
}


char 
IABPDownloadFindItem::Shortcut(uint32 *modifiers) const {
	//if(modifiers)
	//	*modifiers = B_COMMAND_KEY;	
	return (char)NULL;
}


