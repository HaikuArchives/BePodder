#include "Utils.h"
#include <Roster.h>
#include <Application.h>
#include <IconUtils.h>
#include <Path.h>
#include <Resources.h>
#include <View.h>

#include <Screen.h>

#include <kernel/fs_info.h>
#include <kernel/fs_attr.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>

#include "ImageCache.h"


BString app_relative("");

BString
FindAppRelativePath()
{
	app_info theInfo;
	BPath	thePath;
	status_t status = be_app->GetAppInfo(&theInfo);
	
	if(status != B_OK) return "";
	
	thePath.SetTo(&theInfo.ref);
	thePath.GetParent(&thePath);
	return thePath.Path();
	
}


BString
GetAppRelativePath()
{
	if(app_relative=="")
		app_relative=FindAppRelativePath();
	
	return app_relative;
	
}

BString		
GetAppFilename()
{
	app_info theInfo;
	status_t status = be_app->GetAppInfo(&theInfo);
	if(status != B_OK) return "";
	else
	
	return theInfo.ref.name;

}


BBitmap*	
LoadIcon(const char* name){
	BPath icon_path(GetAppRelativePath().String());
	icon_path.Append("icons");
	icon_path.Append(name);
	return ImageCache::GetImage(icon_path.Path(),BString(name));	
}

void		
AddIcon(const char* name,BBitmap* icon){
	ImageCache::AddImage(name,icon);	
}


BBitmap* ResourceVectorToBitmap(const char *resName, float iconSize)
{
	BResources res;
	size_t size;
	app_info appInfo;

	be_app->GetAppInfo(&appInfo);
	BFile appFile(&appInfo.ref, B_READ_ONLY);
	res.SetTo(&appFile);
	BBitmap *aBmp = NULL;
	const uint8* iconData = (const uint8*) res.LoadResource('VICN', resName, &size);

	if (size > 0 ) {
		aBmp = new BBitmap (BRect(0,0, iconSize, iconSize), 0, B_RGBA32);
		status_t result = BIconUtils::GetVectorIcon(iconData, size, aBmp);
		if (result != B_OK) {
			delete aBmp;
			aBmp = NULL;
		}
	}
	return aBmp;
}

bool		
CheckMIME(BString mime,BNode* node){

	size_t len = mime.Length();
	char	buffer[B_MIME_TYPE_LENGTH];
	
	memset(buffer,0,B_MIME_TYPE_LENGTH);
	
	size_t r = node->ReadAttr("BEOS:TYPE",B_MIME_TYPE,0,buffer,len);
	
	if(r!=mime.Length() || mime.Compare(buffer) != 0) return false;
	
	return true;
}

status_t 
ReadAttributeData(BNode* node,const char *name, char **buffer, int32 *size) {
		attr_info info;
		status_t ret = node->GetAttrInfo(name, &info);
		if (ret == B_OK) {
			*buffer = (char *)calloc(info.size, sizeof(char));
			ret = node->ReadAttr(name, info.type, 0, *buffer, info.size);
			if (ret > B_OK) {
				*size = ret;
				ret = B_OK;
				
			} else {
				free(*buffer);
			};
		};
	return ret;
}

status_t 
ReadAttributeInt32(BNode* node,const char *name, int32 *dest) {
		attr_info info;
		status_t ret = node->GetAttrInfo(name, &info);
		if (ret == B_OK) {
			//*buffer = (char *)calloc(info.size, sizeof(char));
			ret = node->ReadAttr(name, info.type, 0, dest, info.size);
			if (ret > B_OK) {
			//	*size = ret;
				ret = B_OK;
				
			} 
		};
	return ret;
}

int32	 	 
ReadAttributeInt32(BNode* node,const char *name){
	int32 ret=0;
	if(ReadAttributeInt32(node,name,&ret)==B_OK){
		return ret;	
	}
	return -1;
}


status_t 
WriteAttributeInt32(BNode* node,const char *name, int32 dest) {
			status_t ret = node->WriteAttr(name, B_INT32_TYPE, 0, &dest, sizeof(int32));
			if (ret > B_OK) ret=B_OK;
			return ret;
}

status_t 
WriteAttributeString(BNode* node,const char *name, const char* string) {
			status_t ret = node->WriteAttr(name, B_STRING_TYPE, 0, string, strlen(string));
			if (ret > B_OK) ret=B_OK;
			return ret;
}


status_t 
ReadAttributeString(BNode* node,const char *name, char **buffer, int32 *size) {
		attr_info info;
		status_t ret = node->GetAttrInfo(name, &info);
		if (ret == B_OK) {
			*buffer = (char *)calloc(info.size + 1, sizeof(char));
			memset(*buffer,'\0',info.size+1);
			ret = node->ReadAttr(name, info.type, 0, *buffer, info.size);
			if (ret > B_OK) {
				*size = ret + 1;
				ret = B_OK;
			} else {
				free(*buffer);
			};
		};
	return ret;
}

BString		 
ReadAttributeString(BNode* node,const char *name){
	char* buffer=NULL;
	int32 size=0;
	if(ReadAttributeString(node,name, &buffer,&size)==B_OK){
		BString ret(buffer,size);
		free(buffer);
		return ret;	
	}
	return "";
}


status_t	
WriteAttributeMessage(BNode* node,const char* name,BMessage* data){
	BMallocIO	malloc;
	status_t ret=data->Flatten(&malloc) ;
	if(	ret == B_OK)	{
 		ret = node->WriteAttr(name,B_ANY_TYPE,0,malloc.Buffer(),malloc.BufferLength());
		if(ret > B_OK) ret=B_OK;
	}
	return ret;
}

status_t	
ReadAttributeMessage(BNode* node,const char* name,BMessage* data){
	
	char *buffer = NULL;
	int32 size = 0;
		
	status_t ret = ReadAttributeData(node,name,&buffer,&size);
	
	if(size>0 && buffer!=NULL) {
		BMemoryIO mem(buffer,size);
		ret = data->Unflatten(&mem);
		free(buffer);
	}
	return ret;
}


void BPDrawString(const char* string, BView* parent, BRect rect,alignment ali,float kTEXT_MARGIN)
{
	float		width = rect.Width() - (2 * kTEXT_MARGIN);
	float		y;
	BFont		font;
	font_height	finfo;

	parent->GetFont(&font);
	font.GetHeight(&finfo);
	y = rect.top + ((rect.Height() - (finfo.ascent + finfo.descent + finfo.leading)) / 2) +
					(finfo.ascent + finfo.descent) - 2;

	switch (ali)
	{
		case B_ALIGN_LEFT:
			parent->MovePenTo(rect.left + kTEXT_MARGIN, y);
			break;

		case B_ALIGN_CENTER:
			parent->MovePenTo(rect.left + kTEXT_MARGIN + ((width - font.StringWidth(string)) / 2), y);
			break;

		case B_ALIGN_RIGHT:
			parent->MovePenTo(rect.right - kTEXT_MARGIN - font.StringWidth(string), y);
			break;
	}
	parent->DrawString(string);
}


// file size:
	const int64 	kKB_SIZE				= 1024;
	const int64 	kMB_SIZE				= 1048576;
	const int64 	kGB_SIZE				= 1073741824;
	const int64 	kTB_SIZE				= kGB_SIZE * kKB_SIZE;
	
BString		
ReadableSize(off_t	size) {
				
				if(size>0 ){		 
				
				char		str[256];
				if (size < kKB_SIZE)
				{
					sprintf(str, "%Ld bytes", size);
				}
				else
				{
					const char*	suffix;
					float 		float_value;
					if (size >= kTB_SIZE)
					{
						suffix = "TB";
						float_value = (float)size / kTB_SIZE;
					}
					else if (size >= kGB_SIZE)
					{
						suffix = "GB";
						float_value = (float)size / kGB_SIZE;
					}
					else if (size >= kMB_SIZE)
					{
						suffix = "MB";
						float_value = (float)size / kMB_SIZE;
					}
					else
					{
						suffix = "KB";
						float_value = (float)size / kKB_SIZE;
					}
					
					sprintf(str, "%.2f %s", float_value, suffix);
					// strip off an insignificant zero so we don't get readings
					// such as 1.00
					char *period = 0;
					char *tmp (NULL);
					for (tmp = str; *tmp; tmp++)
					{
						if (*tmp == '.')
							period = tmp;
					}
					if (period && period[1] && period[2] == '0')
						// move the rest of the string over the insignificant zero
						for (tmp = &period[2]; *tmp; tmp++)
							*tmp = tmp[1];
			}
			return BString(str);
		}
		return "";
}

BString	
ReadableSpeed(float size) {
		
		char		str[256];
		if (size < kKB_SIZE)
		{
			sprintf(str, "%Ld b/s", (off_t)size);
		}
		else
		{
			const char*	suffix;
			float 		float_value;
			if (size >= kTB_SIZE)
			{
				suffix = "TB/s";
				float_value = (float)size / kTB_SIZE;
			}
			else if (size >= kGB_SIZE)
			{
				suffix = "GB/s";
				float_value = (float)size / kGB_SIZE;
			}
			else if (size >= kMB_SIZE)
			{
				suffix = "MB/s";
				float_value = (float)size / kMB_SIZE;
			}
			else
			{
				suffix = "k/s";
				float_value = (float)size / kKB_SIZE;
			}
					
			sprintf(str, "%.2f %s", float_value, suffix);
			// strip off an insignificant zero so we don't get readings
			// such as 1.00
			char *period = 0;
			char *tmp (NULL);
			for (tmp = str; *tmp; tmp++)
			{
				if (*tmp == '.')
					period = tmp;
			}
			if (period && period[1] && period[2] == '0')
				// move the rest of the string over the insignificant zero
				for (tmp = &period[2]; *tmp; tmp++)
					*tmp = tmp[1];
		}
	   		
	   	return BString(str);
}
