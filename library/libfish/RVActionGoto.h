#ifndef  _RVActionGoto_h_
#define  _RVActionGoto_h_
 
#include "RVAction.h"

#include <stdlib.h>
#include <Messenger.h>


class RVActionGoto : public RVAction
{
	public:
		 		RVActionGoto(BHandler* h){hand=h;};
		
		
		virtual 	status_t		Perform(BString param){
				//printf("goto : %s\n",param.String());
			BMessage* msg=new BMessage('goto');
			msg->AddString("goto",param);
			BMessenger(hand).SendMessage(msg,hand);
		}
		virtual	BString		GetDescription(){
			return "Goto";
		}	
		
		private:
			BHandler*	hand;
		
};

#endif
