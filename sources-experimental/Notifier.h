/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef Notifier_H_
#define Notifier_H_

#include <List.h>

#include "ElementListener.h"
#include "ItemProperty.h"

class Notifier
{

public:
   			 Notifier();
    virtual ~Notifier();
   
    
    void 	   AddElementListener(ElementListener*);
    void 	RemoveElementListener(ElementListener*);
  
  
  				
  	virtual void		SetKey(ItemProperty key,void* data,ssize_t size);
		    void		SetKeyString(ItemProperty key,const char* data);
			void		SetKeyInt32(ItemProperty key,int32 data);
 
  			void		NotifyListener(ItemProperty key,void* data,ssize_t size);
 
 private: 
 			BList		fListeners;

};
#endif

//--
