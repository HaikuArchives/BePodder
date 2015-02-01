#ifndef _Action_h_
#define _Action_h_

#include <Message.h>
#include <String.h>

#define ActionID	bigtime_t

class Action 
{
	public:
				 Action(){fActionID=0;};
		virtual	~Action(){};
		
		virtual 	status_t		Perform(BMessage* errors) = 0; 
		virtual	BString		GetDescription() = 0;	
		
				//general purpose ID..
				void		SetActionID(ActionID id){ fActionID=id;}
				ActionID	GetActionID(){ return fActionID;}
		
		private:
				ActionID fActionID;
};

#endif
