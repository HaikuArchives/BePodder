/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _Retriver_H_
#define _Retriver_H_

#include "ItemProperty.h"
#include <String.h>

class Retriver {

	public:
		
	virtual ssize_t		GetData(ItemProperty key,const void** data) = 0;
    virtual int32		CountData()= 0;
    virtual BString		GetDataString(ItemProperty key)= 0;
	virtual int32		GetDataInt32(ItemProperty key)= 0;
	
};

#endif

//..
