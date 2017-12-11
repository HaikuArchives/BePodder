/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "DownloadListItem.h"
#include "ItemProperty.h"

#include <stdlib.h>
#include "BPLocale.h"

#include "Utils.h"
#include "curl/curl.h"
#include "FileStatus.h"
#include "ColumnListView.h"
#include "ColumnTypes.h"
#include "DownloadListView.h"
#include "EpisodeListItem.h"
#include "SubscriptionListItem.h"

DownloadListItem::DownloadListItem(SubscriptionListItem* ch):BRow()
{
	int index = 0;
	time_t time = 0;

	SetField( iconfield =   new BBitmapField(NULL) ,  index++);
	SetField( channel  =   new BStringField(NULL) ,   index++);
	SetField( field		  =   new BStringField(NULL) ,   index++);
	SetField( 				   new BDateField(&time) , 	 index++);
	SetField( sizefield    =   new BSizeField(0), 		 index++);
	SetField( fieldStaus = new FileStatusField(NEW) , index++);
	

	if(ch){	
		channel->SetString(ch->GetTitle().String());	
	}
	
		fShouldMeRemoved = false;
		
		fLinked=NULL;
		fRss.AddElementListener(this);

}

void		
DownloadListItem::LinkToEpisode(EpisodeListItem* item){
	if(fLinked!=NULL)
		debugger("DownloadListItem::fLinked !=NULL, should never happen.");
	fLinked=item;
	fRss.AddElementListener(fLinked);
}

void		
DownloadListItem::Unlink(){
	if(fLinked)
		fRss.RemoveElementListener(fLinked);
	fLinked=NULL;
}
					

void 
DownloadListItem::KeyModified(int32 key, void* data,ssize_t size)
{
	switch(key){

		case ITEM_TITLE:
			 field->SetString((const char*)data);
		break;
		case ITEM_PUBDATE:
		{
			  time_t when = *((time_t*)data);
			  SetField( new BDateField(&when), 3 );
		}
			 
		break;
		
		case  ITEM_ENCLOSURE_LENGTH:
		{
			 off_t size= *((off_t*)data);
	    	 sizefield-> SetSize(size);
		}
		break;
	
		case ITEM_ENCLOSURE_FILE_STATUS:
		{
		
			// BString sdata;
			 int32 val=*((int*)data);
			 FileStatus status = (FileStatus)val;
			 
			 switch(status) 
			 {
			 	case NOT_DOWNLOADED:
			 	case NEW:
			 	case NO_ENCLOSURE:
			 		iconfield->SetBitmap(NULL);
			  	break;
		     	case DOWNLOADED:
		     		iconfield->SetBitmap(LoadIcon("done-micro.png"));
			  	break;
		     	case STOPPED:
		     		iconfield->SetBitmap(LoadIcon("stopped-micro.png"));
			 	 break;
		     	case DOWNLOADING:
		     		iconfield->SetBitmap(LoadIcon("download-micro.png"));
			 	break;
			 	case ENQUEQUED:
			 		iconfield->SetBitmap(LoadIcon("enqueued-micro.png"));
			 	break;
			 	case CONNECTING:	break; //just use the last one!
			 	default:
			 		iconfield->SetBitmap(LoadIcon("error-micro.png"));
			 		//sdata.SetTo(_T("error"));
			 	break; 
		  }
			 
		  fieldStaus->SetFileStatus(status);
//		  if(fList) // TODO FIX THE CLASS BRow in BColumnListView
//		  		fList->UpdateRow(this);
		  Invalidate();
		}
		break;
		case ITEM_ENCLOSURE_FILE_PERCENTAGE:
		{
			int32 val=*((int*)data);
			fieldStaus->SetFilePercentage(val,fLatestSpeed);
		}
		break;
		case ITEM_ENCLOSURE_DOWNLOAD_SPEED:
			 fLatestSpeed= *((float*)data);
		break;
		
		
		default:
		break;
	};	
	
}

DownloadListItem::~DownloadListItem(){
}

float
DownloadListItem::GetLastSpeed(){
	return fLatestSpeed;
}
int32
DownloadListItem::GetPercentage(){
	return fieldStaus->GetPercentage();
}

