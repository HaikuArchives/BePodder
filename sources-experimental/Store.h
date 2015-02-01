/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _Store_H_
#define _Store_H_

#include "ItemProperty.h"
#include <SupportDefs.h>

class Store {

	public:
		
	virtual void	SetKey(ItemProperty key,void* data,ssize_t size) = 0;
    
    virtual void	SetKeyString(ItemProperty key,const char* data) = 0;
	virtual void	SetKeyInt32(ItemProperty key,int32 data) = 0;
	
};

#endif

//..
