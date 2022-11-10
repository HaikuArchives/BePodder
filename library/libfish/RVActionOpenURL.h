#ifndef  _RVActionOpenURL_h_
#define  _RVActionOpenURL_h_
 
#include "RVAction.h"

#include <stdlib.h>
#include <Messenger.h>


class RVActionOpenURL : public RVAction
{
	public:
		 				RVActionOpenURL(BHandler* h){hand=h;};
				void	SetHandler(BHandler* h){hand=h;};
		
		virtual 	status_t		Perform(BString param){
				BMessage* msg=new BMessage('opur');
				msg->AddString("url",param);
				return BMessenger(hand).SendMessage(msg,hand);
		}
		
		virtual	BString		GetDescription(){
			return "OpenURL";
		}	
		
		private:
			BHandler*	hand;
		
};

#endif
