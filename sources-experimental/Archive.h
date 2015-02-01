/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _Archive_H_
#define _Archive_H_

#include "Store.h"
#include "Retriver.h"
#include "ElementListener.h"

class Archive : public Store, public Retriver, public ElementListener {

	public:
	
	// from Store	
	virtual void		SetKey(ItemProperty key,void* data,ssize_t size) = 0;
    virtual void		SetKeyString(ItemProperty key,const char* data) = 0;
	virtual void		SetKeyInt32(ItemProperty key,int32 data) = 0;
	
	
	//from Retriver
	virtual ssize_t		GetData(ItemProperty key,const void** data)= 0;
    virtual int32		CountData()= 0;
    virtual BString	GetDataString(ItemProperty key)= 0;
	virtual int32		GetDataInt32(ItemProperty key)= 0;
	
	//from ElementListener
	virtual void KeyModified(int32 key, void* data, ssize_t numBytes) = 0;
	
};

#endif

//..
