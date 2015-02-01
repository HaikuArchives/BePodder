#ifndef  _RVActionBPOpenURL_h_
#define _RVActionBPOpenURL_h_

#include "RVAction.h"
#include "MainController.h"
#include <stdlib.h>

class RVActionBPOpenURL : public RVAction
{
	public:
		
						RVActionBPOpenURL(MainController* controller){ fController=controller;};
		virtual	~RVActionBPOpenURL(){ };
		
		virtual 	status_t		Perform(BString param){
			fController->OpenURL(param);
			return B_OK;
		}
		virtual	BString		GetDescription(){
			return "RVActionBPOpenURL";
		}	
		
	private:
		MainController*	fController;
		
};

#endif
