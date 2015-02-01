/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "RssItem.h"
#include <String.h>

#include "FileStatus.h"
#include <File.h>
//#include "curl/curl.h"

//#include "MemoryArchive.h"

RssItem::RssItem(){
	SetKeyInt32(ITEM_ENCLOSURE_FILE_STATUS,0);
}
void		
RssItem::SetKey(ItemProperty key,void* data,ssize_t size){

	switch(key){
		case ITEM_ENCLOSURE_FILE_STATUS:
				fStatus = *((FileStatus*)data);
		break;
		case ITEM_GUID:
			fGuid.SetTo((const char*)data,size);
		break;
		case ITEM_ENCLOSURE_URL:
			fEnclosureLink.SetTo((const char*)data,size);
		break;
		case ITEM_ENCLOSURE_LOCALPATH:
			fEnclosurePath.SetTo((const char*)data,size);
		break;
		case ITEM_ENCLOSURE_TYPE:
			fType.SetTo((const char*)data, size);
		break;
		default:break;
	}
	Notifier::SetKey(key,data,size);
}
//.-.
