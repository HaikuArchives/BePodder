/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef Extractor_H_
#define Extractor_H_

#include "Notifier.h"
#include "ItemProperty.h"



class Extractor
{

public:
   
     void 	SetNotifier(Notifier*);
   
      
  protected: 
  		
  				Notifier*	fNotifier;
    void		NotifyNotifier(ItemProperty key,void* data,ssize_t);
    void		NotifyString(ItemProperty key,const char* data);
    void		NotifyInt32(ItemProperty key,int32 data);
  
};
#endif
