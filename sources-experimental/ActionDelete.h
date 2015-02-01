#ifndef _ActionDelete_h_
#define _ActionDelete_h_

#include "Action.h"
#include <String.h>

class ActionDelete : public Action
{
	public:
					 ActionDelete(BString fWho,bool recoursive);
			
			
		status_t	Perform(BMessage* errors);
		BString		GetDescription() { return BString("ActionDelete"); }
		
	private:
		int		del();
		
		BString fWho;
		bool	fRecoursive;
};
#endif
