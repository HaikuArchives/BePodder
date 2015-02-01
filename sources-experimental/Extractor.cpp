/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "Extractor.h"
#include <stdlib.h>

void
Extractor::SetNotifier(Notifier* list)
{
 	fNotifier = list;
}

void
Extractor::NotifyNotifier(ItemProperty key,void* data,ssize_t size){
	fNotifier->SetKey(key,data,size);
}

void		
Extractor::NotifyString(ItemProperty key,const char* data){
	fNotifier->SetKeyString(key,data);
}

void		
Extractor::NotifyInt32(ItemProperty key,int32 data){
	fNotifier->SetKeyInt32(key,data);
}

