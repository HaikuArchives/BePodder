/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _MemoryArchive_H_
#define _MemoryArchive_H_

#include "Archive.h"
#include "Notifier.h"
#include <Message.h>

class MemoryArchive : public Archive {

	public:
	
	virtual				~MemoryArchive(){}	
	
	MemoryArchive &operator=(const MemoryArchive &msg);
	
	// from Store	
	void		SetKey(ItemProperty key,void* data,ssize_t size);
    void		SetKeyString(ItemProperty key,const char* data);
	void		SetKeyInt32(ItemProperty key,int32 data);
	
	
	//from Retriver
	ssize_t		GetData(ItemProperty key,const void** data);
    int32		CountData();
    BString		GetDataString(ItemProperty key);
	int32		GetDataInt32(ItemProperty key);
	
	//utility function.. (MOVE?)
	void		FillListener(Notifier*);
		
	//from ElementListener
	void KeyModified(int32 key, void* data, ssize_t numBytes);
	
	
	void	MakeEmpty();
	
	private:
	
				BMessage fData;
	
};
#endif

//..
