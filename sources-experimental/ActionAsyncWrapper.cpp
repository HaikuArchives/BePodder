#include "ActionAsyncWrapper.h"


ActionAsyncWrapper::ActionAsyncWrapper(Action* action){ 
	SetTargetAndWhat(NULL,0);
	fAction = action;
}

status_t		
ActionAsyncWrapper::Perform(BMessage* errors) {
	if(fAction)
		return fAction->Perform(errors);
	else
		return B_ERROR;
}
