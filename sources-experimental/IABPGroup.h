/*
 *   Copyright (C) 2007 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IABPGroup_h_
#define _IABPGroup_h_

#include "IActionBP.h"
#include "Utils.h"
#include "BPLocale.h"

#define GROUP_NAME_CHANGE	'grnc'

//-------------------------------------------------------------------------------------------------------------------
class IABPGroupCheck : public IActionBP
{
	public:
			
			 	  IABPGroupCheck(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPGroupAdd : public IActionBP
{
	public:
			
			 	  IABPGroupAdd(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPGroupRename : public IActionBP
{
	public:
			
			 	  IABPGroupRename(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPGroupRemove : public IActionBP
{
	public:
			
			 	  IABPGroupRemove(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

#endif
