/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "ElementNotifier.h"
#include <stdlib.h>

void
ElementNotifier::SetElementListener(ElementListener* list)
{
 	fListener = list;
}

void
ElementNotifier::NotifyListener(ItemProperty key,void* data,ssize_t size)
{
	fListener->KeyModified(key,data,size);
}
