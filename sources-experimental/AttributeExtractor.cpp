#include "AttributeExtractor.h"
#include "FileStatus.h"
#include "AttributeMap.h"
#include <Debug.h>
#include <TypeConstants.h>
#include <Message.h>
#include <Bitmap.h>
#include "Utils.h"

void
AttributeExtractor::Parse(BNode *node,bool deep){

	char buf[B_ATTR_NAME_LENGTH];
	
	node->RewindAttrs();
		
	while (node->GetNextAttrName(buf) == B_OK) {
		//printf("> Attr name: %s\n", buf);
		BString attr(buf);
		
		if(attr.ICompare(ATTR_TITLE)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(CHANNEL_TITLE,buf,r);
		}
		else
		if(attr.ICompare(ATTR_WEBLINK)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(CHANNEL_WEBLINK,buf,r);
		}
		else
		if(attr.ICompare(ATTR_IMAGE)==0){
			 
			 //new style
			 
			 char*	buffer;
			 int32	size;
			 status_t err = ReadAttributeData(node,ATTR_IMAGE,&buffer,&size);
			 
			 if(err !=B_OK ) return;
			 
			 BMessage iconMsg;

			 if (iconMsg.Unflatten(buffer) != B_OK) {
			 	free(buffer);
			 	return ;
			 };
			 BBitmap* icon = new BBitmap(&iconMsg);
			 if(!icon) {
			 	free(buffer);
			 	return;
			 }
			 
			 NotifyNotifier(CHANNEL_IMAGE_DATA,(void*)icon,sizeof(BBitmap*));
			 
			 free(buffer);
			 delete icon;
			 
			  
		}
		
		else if(attr.ICompare(ATTR_ITEM_TITLE)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(ITEM_TITLE,buf,r); 
		}
	    else if(attr.ICompare(ATTR_URL)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(CHANNEL_URL,buf,r); 
		}
		else if(attr.ICompare(ATTR_ITEM_GUID)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(ITEM_GUID,buf,r); 
		}
		else if(attr.ICompare(ATTR_ENCLOSURE_URL)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(ITEM_ENCLOSURE_URL,buf,r); 
		}
		else if(attr.ICompare(ATTR_ENCLOSURE_PATH)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(ITEM_ENCLOSURE_LOCALPATH,buf,r); 
		}
		else if(attr.ICompare(ATTR_ENCLOSURE_TYPE)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 buf[r]='\0';
			 NotifyNotifier(ITEM_ENCLOSURE_TYPE,buf,r); 
		}
		
		else if(attr.ICompare(ATTR_ITEM_PUBDATE)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,sizeof(time_t));
			 //time_t time;
			 //memcpy(&time,buf,sizeof(time));
			 //printf("EX DEBUG TIME %ld\n",time);
			 NotifyNotifier(ITEM_PUBDATE,(void*)buf,r); 
		}
		else if(attr.ICompare(ATTR_ITEM_STATUS)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,sizeof(FileStatus));
			 NotifyNotifier(ITEM_ENCLOSURE_FILE_STATUS,(void*)buf,r); 
		}
		else if(attr.ICompare(ATTR_ENCLOSURE_SIZE)==0){
			 size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 NotifyNotifier(ITEM_ENCLOSURE_LENGTH,(void*)buf,r); 
		}
		//this doesn't need to be extracted:
		if(deep){
			if(attr.ICompare(ATTR_ITEM_DESCRIPTION)==0){
				//new style
			 
			 char*	buffer;
			 int32	size;
			 status_t err = ReadAttributeString(node,ATTR_ITEM_DESCRIPTION,&buffer,&size);
			 
			 if(err !=B_OK ) return;
		 	 NotifyNotifier(ITEM_DESCRIPTION,(void*)buffer,size); 
		 	  free(buffer);
			}
			else if(attr.ICompare(ATTR_ITEM_LINK)==0){
			 		size_t r = node->ReadAttr(buf,0,0,buf,B_ATTR_NAME_LENGTH);
			 		buf[r]='\0';
			 		NotifyNotifier(ITEM_LINK,buf,r); 
			}
			else if(attr.ICompare(ATTR_DESCRIPTION)==0){
					 char*	buffer;
			 		 int32	size;
			 		status_t err = ReadAttributeString(node,ATTR_DESCRIPTION,&buffer,&size);
			    	 if(err !=B_OK ) return;
		 	 		NotifyNotifier(CHANNEL_DESCRIPTION,(void*)buffer,size); 
		 	  		free(buffer);
			}
			
		}
	} 
}
