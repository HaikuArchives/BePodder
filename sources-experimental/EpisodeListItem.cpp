/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "EpisodeListItem.h"
#include "ItemProperty.h"

#include <stdlib.h>
#include "BPLocale.h"

#include "Utils.h"
#include "curl/curl.h"
#include "FileStatus.h"
#include "ColumnListView.h"
#include "ColumnTypes.h"


EpisodeListItem::EpisodeListItem():BRow(),isNew(false)
{
	int index = 0;
	time_t time = 0;
	fLatestSpeed = 0;
	
	SetField( iconfield =   new BBitmapField(NULL) ,  index++);
	SetField( field		  =   new BStringField(NULL) ,   index++);
	SetField( 				   new BDateField(&time) , 	 index++);
	SetField( sizefield    =   new BSizeField(0), 		 index++);
	SetField( fieldStaus = new FileStatusField(NEW) , index++);
	
	//fRss.AddElementListener(this);
}


void 
EpisodeListItem::KeyModified(int32 key, void* data,ssize_t size)
{
	printf("key %d %d\n", key , ITEM_ENCLOSURE_LENGTH);
	switch(key)
	{

		case ITEM_TITLE:
		{
			 field->SetString((const char*)data);
		}
		break;
		case ITEM_PUBDATE:
		{
			  time_t when = *((time_t*)data);
			  SetField( new BDateField(&when), 2 );
		}
			 
		break;
		
		case  ITEM_ENCLOSURE_LENGTH:
		{
			 off_t size= *((off_t*)data);
	    	 sizefield-> SetSize(size);
		}
		break;
		
		case ITEM_ENCLOSURE_URL:
		
			if( fieldStaus->GetFileStatus()== NEW )
			 	 iconfield->SetBitmap(LoadIcon("clip.png"));
		break;
		
		case ITEM_ENCLOSURE_FILE_STATUS:
		{
		
			// BString sdata;
			 int32 val=*((int*)data);
			 FileStatus status = (FileStatus)val;
			 
			 if(status!=NEW)
			 	SetIsNew(false);
			 else
			 	SetIsNew(true);
			  
			 switch(status) 
			 {
			 	case NEW:
			 	break;
			 	
			 	case NOT_DOWNLOADED:
			 		iconfield->SetBitmap(LoadIcon("clip.png"));	
			 	break;
			 	
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
			 	case CONNECTING:	break;//just use the last one!
			 	default:
			 		iconfield->SetBitmap(LoadIcon("error-micro.png"));
			 		//sdata.SetTo(_T("error"));
			 	break; 
		  }
			 
			  fieldStaus->SetFileStatus(status);
			  //test
			/* FIX in BRow
			if(fList)
				UpdateRow(this);*/
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

EpisodeListItem::~EpisodeListItem(){
}

void
EpisodeListItem::DrawBackground(	BRect _rect, BView* parent, bool _is_selected, bool _is_focus ) {

//	BRow::DrawBackground(_rect,parent,_is_selected,_is_focus);			
	if(isNew)  {
			parent->SetHighColor(0,0,0);
			parent->FillRect(BRect(_rect.left  + 6, _rect.top + 6,_rect.left + 9,_rect.top + 9));
	}
}

