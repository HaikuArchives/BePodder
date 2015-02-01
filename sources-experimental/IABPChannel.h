/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IABPChannel_h_
#define _IABPChannel_h_

#include "IActionBP.h"
#include "Utils.h"
#include "AddWindow.h"

#include "BPLocale.h"

//request for the AddWindow

//-------------------------------------------------------------------------------------------------------------------
class IABPChannelAddRequest : public IActionBP
{
	public:
			
			 	  IABPChannelAddRequest(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPChannelRemove : public IActionBP
{
	public:
			
			 	  IABPChannelRemove(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
				  
				  char Shortcut(uint32 *modifiers = NULL) const;
};

//-------------------------------------------------------------------------------------------------------------------
class IABPChannelCheck : public IActionBP
{
	public:
			
			 	  IABPChannelCheck(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPChannelWWW: public IActionBP
{
	public:
			
			 	  IABPChannelWWW(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPChannelEnclosureFolder: public IActionBP
{
	public:
			
			 	  IABPChannelEnclosureFolder(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPChannelCheckAll: public IActionBP
{
	public:
			
			 	  IABPChannelCheckAll(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPChannelShowImage: public IActionBP
{
	public:
			
			 	  IABPChannelShowImage(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};



#endif
