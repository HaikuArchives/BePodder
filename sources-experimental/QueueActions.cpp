#include "QueueActions.h"

#include <Message.h>

#include <stdio.h>

#define CHKLOCK if(!IsLocked()) debugger("\nQueueActions must me locked!\n");

QueueActions::QueueActions(const char* name){
	BString n(name);
	n << "'s enemy";
	
	
	fID = spawn_thread(QueueActions::ManageTheQueue,n.String(),B_NORMAL_PRIORITY,this);
	printf("Thread ready [%s] %ld\n",n.String(),fID);
	
	fLock = create_sem(1,name);
	fLocked = false;
	fCurrentAction = NULL;
}

QueueActions::~QueueActions(){
	if(Lock()){
		suspend_thread(fID);
		exit_thread(fID);
		delete_sem(fLock);
	}
}


bool	
QueueActions::Lock(){ 
	status_t value = acquire_sem(fLock) ;
	if(value==B_NO_ERROR){
		fLocked = true;
	}
	return (value == B_NO_ERROR);
}

void	
QueueActions::Unlock(){ 
	fLocked = false;
	release_sem(fLock); 
}
					
void	
QueueActions::AddAction(Action* a){

	//FIX!
	//esiste già un azione con lo stesso ID???
	
	if(Lock()){
		printf("adding %ld - Action name %s\n",fID,a->GetDescription().String());
		fList.AddItem(a);
		if(fList.CountItems() == 1) //Auto-start thread
			resume_thread(fID);
			
	Unlock();
	}
}

Action*	
QueueActions::CurrentAction(){
	CHKLOCK;
	return fCurrentAction;
}

void		
QueueActions::SetCurrentAction(Action* action){
	CHKLOCK;
	fCurrentAction = action;
}
		
int32		
QueueActions::CountActions(){
	CHKLOCK;
	return fList.CountItems();
}

Action*	
QueueActions::ActionAt(int32 pos){
	CHKLOCK;
	fList.ItemAt(pos); 
}

void
QueueActions::RemoveActionAt(int32 pos){
	CHKLOCK;
	fList.RemoveItemAt(pos);
}

bool
QueueActions::IsLocked(){
	return fLocked;
}
					

int32	
QueueActions::ManageTheQueue(void* data) {

	QueueActions *qa=(QueueActions*)data;
	
	printf("Thread started %ld\n",qa->fID);
	
	while(true){
		
		Action *last=NULL;

		if(qa->Lock()) {
			
			printf("Thread executing PID %ld Count %ld\n",qa->fID,qa->fList.CountItems());
			if(qa->fList.CountItems()>0){
				// remove and delete the action.
				last=qa->fList.ItemAt(0);
				qa->fList.RemoveItemAt(0);
				qa->SetCurrentAction(last);
			
			}	else {
			
				last=NULL;
				qa->SetCurrentAction(last);
			}		
			
			qa->Unlock();
		}
		
		if(last)
		{
			// pop the action
			qa->ActionReadyToPerform(last);					
			// perform the action
			BMessage err;
			status_t status = last->Perform(&err); 			
			// do post-perform!
		
			qa->ActionPerformed(last,status,&err);
			
			if(qa->Lock()){
				qa->SetCurrentAction(NULL);
				delete last;
				qa->Unlock();
			}
		
		}
		else {
			
			printf("Thread suspend PID %ld Count %ld\n",qa->fID,qa->fList.CountItems());
			suspend_thread(qa->fID);
		}
	
	} //while

}
