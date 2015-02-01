#include "ActionDelete.h"
#include "Logger.h"
#include <stdlib.h>

ActionDelete::ActionDelete(BString who,bool recoursive) {
	fRecoursive = recoursive;
	fWho=who;			
}
			
status_t 
ActionDelete::Perform(BMessage* errors){
		
		BString commandline;
		commandline << "rm ";
		if(fRecoursive)
			commandline << "-r ";
		commandline << "\"" << fWho << "\"";
		LOG("ActionDelete",liDebug,"\n\nPerforming action [%s]\n\n",commandline.String() );
		int retcode = system(commandline.String());
		
		if(retcode!=0  ){
			if(errors)	errors->AddString("error","Can't delete!");	
			return B_ERROR;
		}
		
		return B_OK;
}
