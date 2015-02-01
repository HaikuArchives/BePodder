#include "QueueFileDownload.h"
#include "ActionDownload.h"
#include "Messenger.h"

void
QueueFileDownload::ActionReadyToPerform(Action* action) 
{
	ActionAsync* ad=dynamic_cast<ActionAsync*>(action);
	if(!ad) return; 
		
	ad->SetTargetAndWhat(fLooper,fMsg);
	
}

void
QueueFileDownload::ActionPerformed(Action* action,status_t state,BMessage* msg)
{

	//ActionAsync* ad=dynamic_cast<ActionAsync*>(action);
	//if(!ad) return;

	BMessage notify(*msg);
	notify.what = fMsg;
	
	if(fLooper)
		BMessenger(fLooper).SendMessage(&notify);
	
	return;
}

void	
QueueFileDownload::SuppressAction(Action* action) {
	ActionDownload* ad=dynamic_cast<ActionDownload*>(action);
	if(!ad) return;

	ad->SetShouldStop(true);
}

//--

