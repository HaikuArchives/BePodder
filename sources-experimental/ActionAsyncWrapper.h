#ifndef  _ActionAsyncWrapper_h_
#define _ActionAsyncWrapper_h_

#include "ActionAsync.h"
#include <Invoker.h>

class ActionAsyncWrapper : public ActionAsync
{
	public:
					ActionAsyncWrapper(Action* action);
					
					status_t		Perform(BMessage* errors); 
	
	private:
					Action*		fAction;
					
};

#endif
