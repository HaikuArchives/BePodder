/*
 *   Copyright (C) 2007 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/
 
//actions for groups

#include "IABPGroup.h"
#include "MainWindow.h"
#include "MainController.h"
#include "TextControlFloater.h"

#include <Roster.h>

#define		LOCKWINDOW					if(fView->Lock()){	
#define		UNLOCKWINDOWERROR	{ fView->Unlock(); return B_ERROR; }
#define		UNLOCKWINDOW				fView->Unlock();}



//-------------------------------------------------------------------------------------------------------------------

IABPGroupCheck::IABPGroupCheck(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("refresh-micro.png"));
	 	   		//SetIcon(IAction::SIZE_48,LoadIcon("refresh-channel-file.png"));
	 	  		//SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("refresh-channel-file-down.png"));
	 	  		
	 	  		SetLabel(_T("Check channels of group")); 
			 	  
}
			 	 
BString 
IABPGroupCheck::GetDescription(){
 	  return _T("Check the podcasts of the selected group for new items");
}
			 	  

status_t 
IABPGroupCheck::Perform(BMessage*){
			LOCKWINDOW
			GroupItem*	group=fView->GetSelectedGroup();
			if(!group) UNLOCKWINDOWERROR
			int32 count = fView->CountSubscriptionOfGroup(group);
			for(int i=0;i<count;i++){
				SubscriptionListItem* row=fView->GetSubscriptionOfGroup(i,group);
				if(row)
					fController->CheckChannel(row->fRef);
			}
			UNLOCKWINDOW
			return B_OK;
}

char 
IABPGroupCheck::Shortcut(uint32 *modifiers) const {
	return 0;
//	if(modifiers)
//		*modifiers = B_COMMAND_KEY | B_SHIFT_KEY;
//	return 'C';
}

//-------------------------------------------------------------------------------------------------------------------

IABPGroupAdd::IABPGroupAdd(MainController* controller,MainWindow* view):IActionBP(controller,view){
 	   		
   		SetIcon(IAction::SIZE_16,LoadIcon("emblem-add.png"));
  		SetLabel(_T("Add a group")); 
			 	  
}
			 	 
BString 
IABPGroupAdd::GetDescription(){
 	  return _T("Add a new group");
}
			 	  

status_t 
IABPGroupAdd::Perform(BMessage*){
	LOCKWINDOW
		fController->AddNewGroup(NULL);
	UNLOCKWINDOW
	return B_OK;
}

char 
IABPGroupAdd::Shortcut(uint32 *modifiers) const {
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------

IABPGroupRename::IABPGroupRename(MainController* controller,MainWindow* view):IActionBP(controller,view){
 	   		
   		SetIcon(IAction::SIZE_16,LoadIcon("emblem-add.png")); //fix this icon..
  		SetLabel(_T("Rename a group")); 
			 	  
}
			 	 
BString 
IABPGroupRename::GetDescription(){
 	  return _T("Rename the selected group");
}
			 	  

status_t 
IABPGroupRename::Perform(BMessage*){
	LOCKWINDOW
			GroupItem*	group=fView->GetSelectedGroup();
			if(!group) UNLOCKWINDOWERROR
			BRect rect;
			if(fView->GetScreenGroupRect(group,&rect)){
				BMessage *info=new BMessage(GROUP_NAME_CHANGE);
				info->AddString("oldname",group->GroupName());
				new TextControlFloater(rect,B_ALIGN_LEFT,be_plain_font,group->GroupName().String(),BMessenger(fView),info);
			}
	UNLOCKWINDOW
	return B_OK;
}

char 
IABPGroupRename::Shortcut(uint32 *modifiers) const {
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------


IABPGroupRemove::IABPGroupRemove(MainController* controller,MainWindow* view):IActionBP(controller,view){
			 	   		
	 	   		SetIcon(IAction::SIZE_16,LoadIcon("delete-micro.png"));
	 	   		//SetIcon(IAction::SIZE_48,LoadIcon("emblem-delete.png"));
	 	  		//SetIcon(IAction::SIZE_48_PRESSED,LoadIcon("emblem-delete-down.png"));
	 	  		
	 	  		SetLabel(_T("Remove a group")); 
}
			 	 
BString 
IABPGroupRemove::GetDescription(){
 	  return _T("Remove a group and delete all the channels");
}
			 	  

status_t 
IABPGroupRemove::Perform(BMessage*){
	
	GroupItem*	group=fView->GetSelectedGroup();
	if(!group) return B_ERROR;
	
	int32 count = fView->CountSubscriptionOfGroup(group);
	
	LOCKWINDOW
	
	if(count>0) {
		BString text;
		text << _TT("alert1_fix..");
		text << "\n\n" << count << " " << "channels" << "\n";
			
		BPAlert* remove = new BPAlert("Remove a group", text.String(),_T("Delete"),_T("Cancel"),NULL,B_WIDTH_AS_USUAL,LoadIcon("emblem-delete.png"));
		int32 result=remove->Go(); //sync..

		if(result==0){
			BPAlert* wait = new BPAlert("Remove a group", "\nRemoving..",NULL,NULL,NULL,B_WIDTH_AS_USUAL,LoadIcon("delete-32.png"));
			wait->Go(NULL); //async..
			
			for(int i=0;i<count;i++){
					SubscriptionListItem* row=fView->GetSubscriptionOfGroup(i,group);
					if(row)
						fController->DeleteChannel(row->fRef);
			}
			wait->PostMessage(B_QUIT_REQUESTED);
		}
	}
	
	fController->RemoveGroup(group->GroupName());
	
	UNLOCKWINDOW

	return B_OK;
} 

char 
IABPGroupRemove::Shortcut(uint32 *modifiers) const {
	return 0;
}
