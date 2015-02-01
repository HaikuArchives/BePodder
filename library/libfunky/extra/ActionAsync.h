#ifndef _ActionAsync_h_
#define _ActionAsync_h_

#include "Action.h"
#include <Invoker.h>

class ActionAsync : public Action , BInvoker
{
	public:
					ActionAsync(BTarget *target = NULL ,uint32 msg = 0);
					
		void		SetTarget(BTarget *tar,uint32 msg);
};

#endif
