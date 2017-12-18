#include "SubscriptionListItem.h"
#include "ItemProperty.h"

#include "ColumnListView.h"
#include "ColumnTypes.h"
//#include <sys_apps/Tracker/Icons.h>
#include <Application.h>
#include "BPLocale.h"
#include "Utils.h"
#include <Entry.h>
#include "malloc.h"

#include <stdio.h>

#define ALTEZZA_BIG 			48
#define ALTEZZA_SMALL 		16 
#define DIMESIONE_ICONA		46

BBitmap* fGenericSubIcon=NULL;

SubscriptionListItem::SubscriptionListItem(entry_ref ref):BRow(ALTEZZA_BIG)
{
	if(!fGenericSubIcon)
		fGenericSubIcon= LoadIcon("file-46.png");
		
	fWeb.SetTo("");

	field = new SubscriptionField("",123);
	field->SetBitmapPointer(fGenericSubIcon);
	SetField( field, 0 );
	
	fChannel.AddElementListener(this);
	
	SetIsNew(false);
	
	extraSetting = columnsState = NULL;
	
	fRef=ref;
	
	fStatusFilter = FILTER_NONE;
}


void				
SubscriptionListItem::SetSize(SubscriptionSize size){
	/*
	//TODO new BColumnView doesn't have the SetHeight method
	switch(size){
		case BIG:
			SetHeight(ALTEZZA_BIG);
		break;
		case SMALL:
			SetHeight(ALTEZZA_SMALL);
		break;
	};
	field->SetSize(size); */
//	printf("SubscriptionListItem SET TITLE %s\n",field->String());
}


bool		
SubscriptionListItem::HasOwnImage(){

	if(field->GetBitmap()==fGenericSubIcon)
	return false;
	
	return true;
}

void 
SubscriptionListItem::KeyModified(int32 key, void* data,ssize_t) {
	
	switch(key)
	{
		case CHANNEL_TITLE:
//				printf("SubscriptionListItem SET TITLE %s\n",(const char*)data);
				field->SetString((const char*)data);
//				printf("SubscriptionListItem SET TITLE %s\n",field->String());
		break;
		
		case CHANNEL_URL:
			fUrl.SetTo((const char*)data); 
		break;
		
		case CHANNEL_WEBLINK:
				fWeb.SetTo((const char*)data);
		break;
		
		case CHANNEL_NEWITEMS:
		{
			int32 c = *((int32*)data);
			field->AddNewCount(c);
		}
		break;
		
		case CHANNEL_RSS_FILE_STATUS:
		{
			int32 c = *((int32*)data);
			field->SetFileStatus((FileStatus)c);	
		}
		break;
		case CHANNEL_IMAGE_DATA:
		{
			BBitmap* icon = (BBitmap*)data;
			if(!icon) return;
			field->SetBitmapPersonal(new BBitmap(icon));
		}
		break;

		default:
		break;
	};	
}


SubscriptionListItem::~SubscriptionListItem(){
	//nothing to do here?
}


/* TODO deprecated
void
SubscriptionListItem::DrawBackground(BRect _rect,
								BView* parent,
								bool _is_selected,
								bool _is_focus ){
									
	BRow::DrawBackground(_rect,parent,_is_selected,_is_focus);			
	if(isNew) {
		parent->SetHighColor(0,0,0);
		parent->FillRoundRect(BRect(_rect.left  + 6, _rect.top + 6,_rect.left + 9,_rect.top + 9),1,1);
	}
}
*/
