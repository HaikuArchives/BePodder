/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "Notifier.h"
#include <stdlib.h>


Notifier::Notifier() {}
Notifier::~Notifier(){}


void	
Notifier::SetKey(ItemProperty key,void* data,ssize_t numBytes)	{
	NotifyListener(key,data,numBytes);	
} 

void		
Notifier::SetKeyString(ItemProperty key,const char* data){
		
		size_t len = strlen(data);
		char copy[ len + 1];
		strncpy(copy,data,len);
		copy[len] = '\0';		
		SetKey(key,(void*)copy, len+1); 
		//SetKey(key,(void*)data,strlen(data)); 
	
}

void		
Notifier::SetKeyInt32(ItemProperty key,int32 data){ 
	SetKey(key,(void*)&data,sizeof(int32)); 
}

void
Notifier::AddElementListener(ElementListener* list){
  fListeners.AddItem((void*)list);
}

void
Notifier::RemoveElementListener(ElementListener* list){
 fListeners.RemoveItem((void*)list);
}


void
Notifier::NotifyListener(ItemProperty key,void* data,ssize_t size)
{
   ElementListener* fListener=NULL;
   for(int l=0; l<fListeners.CountItems();l++)
   {
 	fListener=(ElementListener*)fListeners.ItemAt(l);
    if(fListener) fListener->KeyModified(key,data,size);
   }
}

//--
