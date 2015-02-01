/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _RssItem_H_
#define _RssItem_H_

#include "Notifier.h"
#include "FileStatus.h"


class RssItem : public Notifier { 
	public:
			RssItem();
			
	//from Notifier
	void		SetKey(ItemProperty key,void* data,ssize_t size);
	
	//mine:
	void		SetTitle(const char* data)  { SetKeyString(ITEM_TITLE,data);   };
	void		SetAuthor(const char* data) { SetKeyString(ITEM_AUTHOR,data);  };
	
	void		SetGuid(const char* data)	{ SetKeyString(ITEM_GUID,data); fGuid.SetTo(data);  }	
	void		SetFileStatus(FileStatus fs){ SetKeyInt32(ITEM_ENCLOSURE_FILE_STATUS,(int32)fs); fStatus = fs; }
	void		SetFilePercentage(int32 per){ SetKeyInt32(ITEM_ENCLOSURE_FILE_PERCENTAGE,(int32)per); }
	void		SetDownloadSpeed(float speed){ SetKey(ITEM_ENCLOSURE_DOWNLOAD_SPEED,(void*)&speed,sizeof(float)); }

	//which are the essential?
	//the idea: crate a 'minimal' Store! and use GetData..(); 
	
	BString 	Guid() 	 		 	 		  { return fGuid; }
	BString		EnclosureLink()	  	  { return fEnclosureLink;   }
	BString		EnclosureLocalPath() { return fEnclosurePath; }
	BString		FileType() { return fType; }
	
	FileStatus		GetFileStatus(){ return fStatus; };

	private:
		BString fGuid;
		BString fEnclosureLink;	
		BString	fEnclosurePath;
		BString fType;
		FileStatus	fStatus;
};
#endif
