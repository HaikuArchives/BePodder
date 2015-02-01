/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#define DirectoryItem_H_
#define DirectoryItem_H_

#include "IconTextItem.h"

class DirectoryItem : public IconTextItem {

	public:
		DirectoryItem(const char *text, BBitmap *icon = NULL,const char *linkz = NULL,BPoint spiaz=BPoint(0,0))
		:IconTextItem(text,icon)
		{	fLink.SetTo(linkz); SetSpiaz(spiaz); }
		
		BString	Link(){ return fLink; }
	private:
		BString	fLink;


};
