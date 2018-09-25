/*
 *   Copyright (C) 2007 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/
 
//actions for groups
#include <Catalog.h>
#include "IABPGroup.h"
#include "MainWindow.h"
#include "MainController.h"
#include "TextControlFloater.h"

#include <Roster.h>

#define		LOCKWINDOW					if(fView->Lock()){	
#define		UNLOCKWINDOWERROR	{ fView->Unlock(); return B_ERROR; }
#define		UNLOCKWINDOW				fView->Unlock();}

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "IABPGroup"

//-------------------------------------------------------------------------------------------------------------------

IABPGroupCheck::IABPGroupCheck(MainController* controller,MainWindow* view):IActionBP(controller,view){
	SetLabel(B_TRANSLATE("Check channels of group"));
}
			 	 
BString 
IABPGroupCheck::GetDescription(){
	return B_TRANSLATE("Check the podcasts of the selected group for new episodes");
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
	SetLabel(B_TRANSLATE("Add group"));
}
			 	 
BString 
IABPGroupAdd::GetDescription(){
	return B_TRANSLATE("Add a new group");
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
	SetLabel(B_TRANSLATE("Rename group"));
}
			 	 
BString 
IABPGroupRename::GetDescription(){
	return B_TRANSLATE("Rename the selected group");
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
	SetLabel(B_TRANSLATE("Remove group"));
}
			 	 
BString 
IABPGroupRemove::GetDescription(){
	return B_TRANSLATE("Remove a group and delete all the channels");
}
			 	  

status_t 
IABPGroupRemove::Perform(BMessage*){
	
	GroupItem*	group=fView->GetSelectedGroup();
	if(!group) return B_ERROR;
	
	int32 count = fView->CountSubscriptionOfGroup(group);
	int32 result = 0;
	LOCKWINDOW
	
	if(count>0) {
		BString text;
		text << B_TRANSLATE("You are going to remove the group:\n\n%groupname%\n\nWith %numberofchannel% channels.\n");
		text.ReplaceFirst("%groupname%", group->GroupName());
		BString countString;
		countString << count;
		text.ReplaceFirst("%numberofchannel%", countString);

		BPAlert* remove = new BPAlert("Remove a group", text.String(),B_TRANSLATE("Delete"),B_TRANSLATE("Cancel"),NULL,B_WIDTH_AS_USUAL, ResourceVectorToBitmap("emblem-delete", 32));
		result = remove->Go(); //sync..

		if(result == 0) {
			BPAlert* wait = new BPAlert("Remove a group", B_TRANSLATE("\nRemoving..."),NULL,NULL,NULL,B_WIDTH_AS_USUAL, ResourceVectorToBitmap("emblem-delete", 32));
			wait->Go(NULL); //async..
			
			for(int i=0;i<count;i++){
					SubscriptionListItem* row=fView->GetSubscriptionOfGroup(0, group);
					if(row)
						fController->DeleteChannel(row->fRef);
			}
			wait->PostMessage(B_QUIT_REQUESTED);
		}
	}
	if (result == 0)
		fController->RemoveGroup(group->GroupName());
	
	UNLOCKWINDOW

	return B_OK;
} 

char 
IABPGroupRemove::Shortcut(uint32 *modifiers) const {
	return 0;
}
