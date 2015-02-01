/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _RssChannel_H_
#define _RssChannel_H_

#include "Notifier.h"
#include "FileStatus.h"

class RssChannel : public Notifier {

	public:
	    		RssChannel();			
	void		SetTitle(const char* data)  {   SetKeyString(CHANNEL_TITLE,data);   };
	void		SetURL(const char* data)	{  	SetKeyString(CHANNEL_URL ,data);   };
	void		SetFileStatus(FileStatus fs){ 	SetKeyInt32(CHANNEL_RSS_FILE_STATUS,(int32)fs); }

};
#endif
