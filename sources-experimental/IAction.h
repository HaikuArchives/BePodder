/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IAction_h_
#define _IAction_h_

#include "Action.h"
#include <Bitmap.h>

class IAction : public Action
{
	public:
			
			enum IconType {
			    	 SIZE_16 = 0,
					 SIZE_48,
					 SIZE_48_PRESSED,
					 //.... add other there!
					 SIZE_HOWMANY
					};
	
				 	  IAction():Action(){ memset(fIcons,0, sizeof(BBitmap*) * SIZE_HOWMANY); };
		
		virtual	~IAction(){};
		
		BBitmap*		GetIcon(IconType type){ return fIcons[type]; }
		void				SetIcon(IconType type,BBitmap* icon){ fIcons[type]=icon; } 
		
		BString			GetLabel()  { return fLabel;}	
		void				SetLabel(BString label){ fLabel=label;}
			
		BMessage	*	CopyMessage() { return new BMessage(*fMessage);}
		BMessage	*	GetMessage() { return fMessage;}
		void				SetMessage(BMessage* msg){ fMessage=msg;}
		
		virtual	BString		GetDescription(){ return "";};
		
		virtual char Shortcut(uint32 *) const { return 0; }
				
		private:
				BBitmap* fIcons[SIZE_HOWMANY];
				BString	fLabel;
				BMessage*	fMessage;
};

#endif
