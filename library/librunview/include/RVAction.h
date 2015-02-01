#ifndef  _RVAction_h_
#define _RVAction_h_

class RVAction 
{
	public:
		
		virtual	~RVAction(){};
		
		virtual 	status_t		Perform(BString param) = 0; 
		virtual	BString		GetDescription() = 0;	
		
};

#endif
