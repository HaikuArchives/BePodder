#ifndef _ActionAsync_h_
#define _ActionAsync_h_

#include "Action.h"
#include <Invoker.h>

class ActionAsync : public Action , public BInvoker
{
	public:
					ActionAsync(BHandler *target = NULL ,uint32 what = 0);
					
		void		SetTargetAndWhat(BHandler *tar,uint32 what);
};

#endif
