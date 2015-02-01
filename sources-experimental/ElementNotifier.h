/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef ElementNotifier_H_
#define ElementNotifier_H_

#include <Message.h>
#include <String.h>
#include <List.h>
#include <Debug.h>

#include "ElementListener.h"
#include <ObjectList.h>


#include "ItemProperty.h"



class ElementNotifier
{

public:
   
     void 	SetElementListener(ElementListener*);
   
      
  protected: 
  		
  				ElementListener*	fListener;
    void		NotifyListener(ItemProperty key,void* data,ssize_t);
  
};
#endif
