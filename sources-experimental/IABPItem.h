/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IABPItem_h_
#define _IABPItem_h_

#include "IActionBP.h"
#include "Utils.h"


//-------------------------------------------------------------------------------------------------------------------
class IABPItemRemove : public IActionBP
{
	public:
			
			 	  IABPItemRemove(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPItemStop : public IActionBP
{
	public:
			
			 	  IABPItemStop(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPItemDownload : public IActionBP
{
	public:
			
			 	  IABPItemDownload(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
		
				 char Shortcut(uint32 *modifiers = NULL) const;
};

//-------------------------------------------------------------------------------------------------------------------
class IABPItemPlay: public IActionBP
{
	public:
			
			 	  IABPItemPlay(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPItemWWW: public IActionBP
{
	public:
			
			 	  IABPItemWWW(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPItemEnclosureFolder: public IActionBP
{
	public:
			
			 	  IABPItemEnclosureFolder(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

#endif
