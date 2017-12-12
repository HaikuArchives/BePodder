#include "AttributeDumper.h"
#include "ItemProperty.h"
#include "TypeConstants.h"
#include "AttributeMap.h"

#include <malloc.h>
#include <stdio.h>
#include <File.h>
#include <Directory.h>
#include <NodeInfo.h>
#include "FileStatus.h"
#include "Utils.h"

//  from the BeBook:
//  	The value of an indexed attribute can be, at most, 255 bytes.
//	from the ITunesRSS format:
//  	All fields will be truncated to 255 unicode characters, except for <itunes:summary>

#define	INDEX_LIMIT_LENGHT	255

AttributeDumper::AttributeDumper(entry_ref* ref) {

	fRef=*ref;
	//BFile *fFile = new BFile(&fRef,B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	fNode = new BNode(ref); //fFile;
}
/*
AttributeDumper::AttributeDumper(node_ref* nref) {

		BDirectory *fDir = new BDirectory(nref);
		fNode = fDir;
}
*/
AttributeDumper::AttributeDumper(entry_ref* ref,bool asdir){

	fNode = NULL;
	
	//new name
	BString filename;
	filename << real_time_clock_usecs();
	//
		
	BDirectory dir(ref);

	fPath.SetTo(&dir,NULL);
	fPath.Append(filename.String());
	
	if(!asdir)
	{

		BFile *fFile = new BFile(fPath.Path(),B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		fNode = fFile;
		
		BNodeInfo(fNode).SetType(MIME_ITEM);
		
		get_ref_for_path(fPath.Path(),&fRef);
	}
	else
	{
		BDirectory *fDir = new BDirectory(dir);
		dir.CreateDirectory(fPath.Path(),fDir);
		fNode = fDir;
		
		BNodeInfo(fNode).SetType(MIME_CHANNEL);
		
		printf("New directory Path [%s]\n",fPath.Path());
		
		get_ref_for_path(fPath.Path(),&fRef);
	}
	
	
}
AttributeDumper::~AttributeDumper(){
	if(fNode) delete fNode;
}
void 
AttributeDumper::KeyModified(int32 key, void* data, ssize_t numBytes){

	
	
	switch(key)
	{
		case ITEM_TITLE:
			_writeAttr(ATTR_ITEM_TITLE,BString((const char*)data,numBytes));
		break;
		case ITEM_GUID:
			_writeAttr(ATTR_ITEM_GUID,BString((const char*)data,numBytes));
		break;
		case ITEM_LINK:
			_writeAttr(ATTR_ITEM_LINK,BString((const char*)data,numBytes));
		break;
		case ITEM_CATEGORY:
			_writeAttr(ATTR_CATEGORY,BString((const char*)data,numBytes));
		break;
		
		case ITEM_KEYWORDS:
			_writeAttr(ATTR_ITEM_KEYWORDS,BString((const char*)data,numBytes));
		break;
		
		case ITEM_PUBDATE:
		{
			time_t when = *((time_t*)data);
			if(fNode) fNode->WriteAttr(ATTR_ITEM_PUBDATE,B_TIME_TYPE,0,&when,sizeof(when));
		}
		break;
		case ITEM_ENCLOSURE_LENGTH:
		{
			off_t size = *((off_t*)data);
			if(fNode) fNode->WriteAttr(ATTR_ENCLOSURE_SIZE,B_OFF_T_TYPE,0,&size,sizeof(size));
		}
		break;
		
		case ITEM_ENCLOSURE_LOCALPATH:
			_writeAttr(ATTR_ENCLOSURE_PATH,BString((const char*)data,numBytes));
		break;
				
		case ITEM_ENCLOSURE_URL:
			_writeAttr(ATTR_ENCLOSURE_URL,BString((const char*)data,numBytes));
		break;
		case ITEM_ENCLOSURE_TYPE:
			_writeAttr(ATTR_ENCLOSURE_TYPE,BString((const char*)data,numBytes));
		break;
		case ITEM_ENCLOSURE_FILE_STATUS:{
			FileStatus fs = *((FileStatus*)data);
			if(fNode) fNode->WriteAttr(ATTR_ITEM_STATUS,B_INT32_TYPE,0,&fs,sizeof(FileStatus));
		}
		break;
		
		case ITEM_DESCRIPTION:
			_writeAttr(ATTR_ITEM_DESCRIPTION,BString((const char*)data,numBytes),false);
		break;
		
		// CHANNEL:
		
		case CHANNEL_TITLE:
			_writeAttr(ATTR_TITLE,BString((const char*)data,numBytes));
		break;
		case CHANNEL_WEBLINK:
			_writeAttr(ATTR_WEBLINK,BString((const char*)data,numBytes));
		break;
		case CHANNEL_URL:
			_writeAttr(ATTR_URL,BString((const char*)data,numBytes));
		break;
		case CHANNEL_CATEGORY:
			_writeAttr(ATTR_CATEGORY,BString((const char*)data,numBytes));
		break;
		
		case CHANNEL_IMAGE_DATA:
			_addBitmap((BBitmap*)data);
		break;
		case CHANNEL_DESCRIPTION:
			_writeAttr(ATTR_DESCRIPTION,BString((const char*)data,numBytes),false);
		break;
		case CHANNEL_NEWITEMS:
		{
//			int32 c = *((int32*)data);
		
		}
		break;
		case CHANNEL_RSS_FILE_STATUS:
		{
//			int32 c = *((int32*)data);
			
		}
		break;
		default:
		break;
	};	

}

void	
AttributeDumper::_writeAttr(const char* name,BString data,bool cut){
	 if(cut) data.Truncate(INDEX_LIMIT_LENGHT);	 
  	 if(fNode) fNode->WriteAttr(name,B_STRING_TYPE,0,data.String(),data.Length());

}

status_t	
AttributeDumper::_addBitmap(BBitmap *icon) {


	char *buffer = NULL;
	int32 size = -1;
	BMessage flattenedIcon;
	status_t ret = B_ERROR;
	BMessage iconMsg;

	if (icon->Archive(&flattenedIcon) != B_OK) return ret;

	size = flattenedIcon.FlattenedSize();
	buffer = (char *)calloc(size, sizeof(char));
	if (flattenedIcon.Flatten(buffer, size) == B_OK) {
		if(fNode) ret = fNode->WriteAttr(ATTR_IMAGE, B_MESSAGE_TYPE, 0, buffer, size);
	};
		
	free(buffer);
	return ret;
};

//__
