/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	 Interface for something that reacts aftwer a change
*/

#ifndef _ElementListener_h_
#define _ElementListener_h_

#include <String.h>

class ElementListener
{
	public:
		virtual void KeyModified(int32 key, void* data,ssize_t numBytes) = 0;		
};
#endif
