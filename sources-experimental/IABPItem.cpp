/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/ 

//actions for items
#include <Catalog.h>
#include "IABPItem.h"
#include "MainWindow.h"
#include "MainController.h"

#define		LOCKWINDOW					if(fView->Lock()){	
#define		UNLOCKWINDOWERROR	{ fView->Unlock(); return B_ERROR; }
#define		UNLOCKWINDOW				fView->Unlock();}

//-------------------------------------------------------------------------------------------------------------------
#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "IABPItem"

IABPItemRemove::IABPItemRemove(MainController* controller,MainWindow* view):IActionBP(controller,view){

	 	   		SetIcon(IAction::SIZE_48,LoadIcon("emblem-delete.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("emblem-delete-down.png"));
				SetIcon(IAction::SIZE_16,ResourceVectorToBitmap("emblem-delete", 16));
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("emblem-delete", 24));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Remove item")); 
}
			 	 
BString 
IABPItemRemove::GetDescription(){
 	  return B_TRANSLATE("Delete the item and the enclosure");
}
			 	  

status_t 
IABPItemRemove::Perform(BMessage*){
	
	EpisodeListItem* row=fView->GetSelectedEpisode();
	if(!row) return B_ERROR;

	BString firstTitle(row->GetTitle() );
	
	BList selection;
	selection.AddItem((void*)row);
	while(row != NULL){
		row=fView->GetSelectedEpisode(row);
		if(row) 
				selection.AddItem((void*)row);
	}


	BString text;
	text << B_TRANSLATE("\nYou are going to remove the selected item.\n\n"
		"Are you sure?\n(You will lose any enclosed file if there are any.)");
	if(selection.CountItems() == 1) 
		text << "\n\n" << firstTitle << "\n";
	else
		text << "\n\n" << selection.CountItems() << " " << "episodes\n";
		
	BPAlert* remove = new BPAlert("Remove an item", text.String(),B_TRANSLATE("Delete"),B_TRANSLATE("Cancel"),NULL,B_WIDTH_AS_USUAL,LoadIcon("emblem-delete.png"));
	int32 result=remove->Go(); //sync..
	
	LOCKWINDOW
	
	EpisodeListView*	elv= fView->GetEpisodeListView();
	int32 removed=0;
	
	if(result==0){
		
		BPAlert* wait = new BPAlert("Remove a item", "\nRemoving..",NULL,NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("delete-32.png"));
		wait->Go(NULL); //async..
		
		for(int i=0;i<selection.CountItems();i++){
				EpisodeListItem* row=(EpisodeListItem*)selection.ItemAt(i);
				if(fController->DeleteEpisode(row->fRef)) removed--;
				elv->RemoveRow(row); //FIX: nnn c'e' un metodo in fView?			
				delete row; //goodbye
		}
		
		wait->PostMessage(B_QUIT_REQUESTED);
		
		elv->DeselectAll();
		SubscriptionListItem* sub=fView->GetSelectedSubscription();
		if(sub) {
			sub->AddNewCount(removed);
			fController->SetSelectedItem(NULL,&sub->fRef);
		}
		else
			fController->SetSelectedItem(NULL,NULL);		
	}

	UNLOCKWINDOW
	return B_OK;
} 

char 
IABPItemRemove::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return 'R';
}

//-------------------------------------------------------------------------------------------------------------------


IABPItemStop::IABPItemStop(MainController* controller,MainWindow* view):IActionBP(controller,view){

	 	   		SetIcon(IAction::SIZE_48,LoadIcon("emblem-stop.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("emblem-stop-down.png"));

				SetIcon(IAction::SIZE_16,ResourceVectorToBitmap("emblem-stop", 16));
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("emblem-stop", 24));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Stop download")); 
}
			 	 
BString 
IABPItemStop::GetDescription(){
 	  return B_TRANSLATE("Stop the item download");
}
			 	  

status_t 
IABPItemStop::Perform(BMessage*){
		
	LOCKWINDOW
	
	EpisodeListItem* row=fView->GetSelectedEpisode();
	while(row != NULL){
		fController->StopDownloadItem(row->fRef);
		row=fView->GetSelectedEpisode(row);
	}
		
	UNLOCKWINDOW	
	return B_OK;
} 

char 
IABPItemStop::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return 'S';
}

//-------------------------------------------------------------------------------------------------------------------


IABPItemDownload::IABPItemDownload(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		

				SetIcon(IAction::SIZE_16,ResourceVectorToBitmap("emblem-download", 16));

	 	   		SetIcon(IAction::SIZE_48,LoadIcon("emblem-download.png"));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("emblem-download-down.png"));
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("emblem-download", 24));
	 	  		SetLabel(B_TRANSLATE("(Re)start download")); 
}
			 	 
BString 
IABPItemDownload::GetDescription(){
 	  return B_TRANSLATE("Download the selected episode");
}
			 	  

status_t 
IABPItemDownload::Perform(BMessage*){
		
	LOCKWINDOW
	EpisodeListItem* row=fView->GetSelectedEpisode();
	while(row != NULL){
		fController->DownloadItem(row->fRef,row);
		row=fView->GetSelectedEpisode(row);		
	}
	UNLOCKWINDOW	
	return B_OK;
} 


char 
IABPItemDownload::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;
	return 'D';
}


//-------------------------------------------------------------------------------------------------------------------


IABPItemPlay::IABPItemPlay(MainController* controller,MainWindow* view):IActionBP(controller,view){

	 	   		SetIcon(IAction::SIZE_48,LoadIcon("emblem-play.png"));

				SetIcon(IAction::SIZE_16,ResourceVectorToBitmap("emblem-play", 16));
				SetIcon(IAction::SIZE_24,ResourceVectorToBitmap("emblem-play", 24));
	 	  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("emblem-play-down.png"));
	 	  		
	 	  		SetLabel(B_TRANSLATE("Play enclosure")); 
}
			 	 
BString 
IABPItemPlay::GetDescription(){
 	  return B_TRANSLATE("Play the selected episode");
}
			 	  

status_t 
IABPItemPlay::Perform(BMessage*){
		
		LOCKWINDOW
		EpisodeListItem* row=fView->GetSelectedEpisode();
		while(row != NULL){
			fController->PlayItem(row->fRef);
			row=fView->GetSelectedEpisode(row);		
		}
		UNLOCKWINDOW	
		
		return B_OK;
} 

char 
IABPItemPlay::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;	
	return 'E';
}


//-------------------------------------------------------------------------------------------------------------------

IABPItemWWW::IABPItemWWW(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
   		SetIcon(IAction::SIZE_16,LoadIcon("www-micro.png"));
   		SetIcon(IAction::SIZE_48,LoadIcon("www-channel-file.png"));
  		SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("www-channel-file-down.png"));
  		
  		SetLabel(B_TRANSLATE("Open webpage")); 
			 	  
}
			 	 
BString 
IABPItemWWW::GetDescription(){
 	  return B_TRANSLATE("Show the news homepage");
}
			 	  

status_t 
IABPItemWWW::Perform(BMessage*){

	EpisodeListItem* row=fView->GetSelectedEpisode();
	while(row != NULL){
		fController->WWWItem(row->fRef);
		row=fView->GetSelectedEpisode(row);		
	}
	return B_OK;
}


char 
IABPItemWWW::Shortcut(uint32 *modifiers) const {
	if(modifiers)
		*modifiers = B_COMMAND_KEY;	
	return 'H';
}

