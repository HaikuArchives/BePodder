#include "ActionAsync.h"


ActionAsync::ActionAsync(BHandler *target  ,uint32 what ){ 
	SetTargetAndWhat(target,what);
}


void
ActionAsync::SetTargetAndWhat(BHandler *tar,uint32 what){ 
	SetTarget(tar); 
	SetMessage(new BMessage(what));
}
