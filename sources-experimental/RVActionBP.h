#ifndef  _RVActionBP_h_
#define _RVActionBP_h_

#include "RVAction.h"
#include "IActionManagerBP.h"

extern IActionManagerBP action_manager;

#include <stdlib.h>

class RVActionBP : public RVAction
{
	public:
		
		virtual	~RVActionBP(){};
		
		virtual 	status_t		Perform(BString param){
			int pos = atoi(param.String());
			if(pos>0)
				action_manager.FireAction(pos);
				return B_OK;
		}
		virtual	BString		GetDescription(){
			return "RVActionBP";
		}	
		
};

#endif
