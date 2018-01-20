/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IABPDownload_h_
#define _IABPDownload_h_

#include "IActionBP.h"
#include "Utils.h"


//-------------------------------------------------------------------------------------------------------------------
class IABPDownloadRemove : public IActionBP
{
	public:
			
			 	  IABPDownloadRemove(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPDownloadStop : public IActionBP
{
	public:
			
			 	  IABPDownloadStop(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
class IABPDownloadDownload : public IActionBP
{
	public:
			
			 	  IABPDownloadDownload(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
		
				 char Shortcut(uint32 *modifiers = NULL) const;
};

//-------------------------------------------------------------------------------------------------------------------
class IABPDownloadPlay: public IActionBP
{
	public:
			
			 	  IABPDownloadPlay(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------

class IABPDownloadFindItem: public IActionBP
{
	public:
			
			 	  IABPDownloadFindItem(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};

//-------------------------------------------------------------------------------------------------------------------
/*
class IABPDownloadEnclosureFolder: public IActionBP
{
	public:
			
			 	  IABPDownloadEnclosureFolder(MainController* controller = NULL,MainWindow* view = NULL);
			 	  			 	  
			 	  BString GetDescription();
			 	  
			 	  status_t Perform(BMessage*);
			 	  
			 	  char Shortcut(uint32 *modifiers = NULL) const;
		
};
*/
#endif
