#include "ActionAsync.h"


ActionAsync::ActionAsync(BTarget *target = NULL ,uint32 msg = 0){ SetTarget(target,msg);};


void
ActionAsync::SetTarget(BTarget *tar,uint32 msg){ SetTarget(tar); SetMessage(new BMessage(msg);};
