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
	
	switch(size){
		case BIG:
			SetHeight(ALTEZZA_BIG);
		break;
		case SMALL:
			SetHeight(ALTEZZA_SMALL);
		break;
	};
	field->SetSize(size);
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
			if(!icon) return; //safety
			//here resizing please!
			//quale è il lato + lungo?
			
			BBitmap* nicon;
			float h  =	icon->Bounds().Height();
			float w  =icon->Bounds().Width();
			
			if( h == w )			
				nicon = rescale_bitmap(icon,DIMESIONE_ICONA,DIMESIONE_ICONA);
			else 
			{
				if(h>w)
				{
					float diff = w/h;
					nicon = rescale_bitmap(icon,DIMESIONE_ICONA*diff,DIMESIONE_ICONA);
				}
				else
				{
					float diff = h/w;
					nicon = rescale_bitmap(icon,DIMESIONE_ICONA,DIMESIONE_ICONA*diff);
				}
				
			}
			
			field->SetBitmapPersonal(nicon);
		}
		break;

		default:
		break;
	};	
}


SubscriptionListItem::~SubscriptionListItem(){
	//nothing to do here?
}


BBitmap* 
SubscriptionListItem::rescale_bitmap( BBitmap * original, int32 width, int32 height )
{
	
	float starting_width = original->Bounds().Width()+1;
	float starting_height = original->Bounds().Height()+1;

	float new_width = width; //round(starting_width * new_settings->width_coefficient);
	float new_height = height; //round(starting_height * new_settings->height_coefficient);

	// Create a new bitmap here and copy it applying scaling.
	// But first create an intermediate bitmap for scaling in one direction only.
	// Remember that the returned bitmap must accept views	
	// First scale the width.	
	// If the new size is the same as old return the original
	if ((new_width == starting_width) && (new_height == starting_height))
		return new BBitmap(original);



	BBitmap *intermediate_bitmap;
	if (new_width != starting_width) {
		intermediate_bitmap = new BBitmap(BRect(0,0,new_width-1,starting_height-1),B_RGB_32_BIT);
		if (intermediate_bitmap->IsValid() == FALSE)
		#ifdef ZETA
			throw bad_alloc();
		#else
			return NULL;
		#endif
		uint32 *target_bits = (uint32*)intermediate_bitmap->Bits();
		int32 target_bpr = intermediate_bitmap->BytesPerRow()/4;
		uint32 *source_bits = (uint32*)original->Bits();
		int32 source_bpr = original->BytesPerRow()/4;
		int32 bottom = (int32)original->Bounds().bottom;
		float diff = (starting_width)/(new_width);
		float accumulation = 0;
	
		
		if (diff<1) {
			// Enlarge in x direction.
			for (int32 y=0;y<=bottom;y++) {
				accumulation = 0;
				for (int32 x=0;x<target_bpr;x++) {
					// This does not calculate correct values because mix_2_pixels does not
					// take rounding errors into account.
	//				*target_bits++ = mix_2_pixels(*(source_bits + (int32)floor(accumulation)),*(source_bits + (int32)ceil(accumulation)),ceil(accumulation)-accumulation);					
					*target_bits++ = mix_2_pixels_fixed(*(source_bits + (int32)floor(accumulation)),*(source_bits + (int32)ceil(accumulation)),(uint32)(32768*(ceil(accumulation)-accumulation)));
					
					accumulation += diff;
				}
				source_bits += source_bpr;
				
			}
		}
		else if (diff>1) {
			// Make smaller in x direction.
			diff = (starting_width-1)/new_width;	// Why this line???
			for (int32 y=0;y<=bottom;y++) {
				accumulation = 0;
				for (int32 x=0;x<target_bpr;x++) {
					// Here we average the original pixels between accumulation and accumulation+diff.
					// The pixels at end get a little lower coefficients than the other pixels.
					// But for now we just settle for averaging the pixels between floor(accumulation)
					// and floor(accumulation+diff):
					float coeff = 1.0;
					float coeff_diff = 1.0/(floor(accumulation+diff)-floor(accumulation));
					uint32 target_value = 0x00000000;
					for (int32 i=(int32)floor(accumulation);i<floor(accumulation+diff);i++) {
						// This does not calculate correct values because mix_2_pixels does not
						// take rounding errors into account.
	//					target_value = mix_2_pixels(*(source_bits + i),target_value,coeff);
						target_value = mix_2_pixels_fixed(*(source_bits + i),target_value,(uint32)(32768*coeff));
						coeff -= coeff_diff;
					}
					*target_bits++ = target_value;
					accumulation += diff;			
				}
				source_bits += source_bpr;
				
			}	
		}
		else {
			for (int32 y=0;y<=bottom;y++) {
				for (int32 x=0;x<target_bpr;x++) {
					// Just copy it straight
					*target_bits++ = *source_bits++;		
				}
				
			}		
		}		
	}	
	else {
		intermediate_bitmap = original;
	}

	if (new_height != starting_height) {
		BBitmap *new_bitmap = new BBitmap(BRect(0,0,new_width-1,new_height-1),B_RGB_32_BIT);
		if (new_bitmap->IsValid() == FALSE)
		#ifdef ZETA
			throw bad_alloc();
		#else
			return NULL;
		#endif	
		uint32 *target_bits = (uint32*)new_bitmap->Bits();
		int32 target_bpr = new_bitmap->BytesPerRow()/4;
		uint32 *source_bits = (uint32*)intermediate_bitmap->Bits();
		int32 source_bpr = intermediate_bitmap->BytesPerRow()/4;
		int32 bottom = (int32)new_bitmap->Bounds().bottom;
		float diff = (starting_height-1)/(new_height);
		float accumulation = 0;
		if (diff<1) {
			// Make larger in y direction.
			for (int32 y=0;y<=bottom;y++) {
				for (int32 x=0;x<target_bpr;x++) {
//					*target_bits++ = mix_2_pixels(*(source_bits + (int32)floor(accumulation)*source_bpr),*(source_bits + (int32)ceil(accumulation)*source_bpr),ceil(accumulation)-accumulation);					
					*target_bits++ = mix_2_pixels_fixed(*(source_bits + (int32)floor(accumulation)*source_bpr),*(source_bits + (int32)ceil(accumulation)*source_bpr),(uint32)(32768*(ceil(accumulation)-accumulation)));
					source_bits++;
				}
				source_bits -= source_bpr;
				accumulation += diff;
				
			}
		}
		else if (diff>1) {
			// Make smaller in y direction.
			diff = (starting_height-1)/new_height;
			accumulation = 0;
			for (int32 y=0;y<=bottom;y++) {
				for (int32 x=0;x<target_bpr;x++) {
					// Here we average the original pixels between accumulation and accumulation+diff.
					// The pixels at end get a little lower coefficients than the other pixels.
					// But for now we just settle for averaging the pixels between floor(accumulation)
					// and floor(accumulation+diff):
					uint32 target_value = 0x00000000;
					float coeff = 1.0;
					float coeff_diff = 1.0/(floor(accumulation+diff)-floor(accumulation));
					for (int32 i=(int32)floor(accumulation);i<floor(accumulation+diff);i++) {
//						target_value = mix_2_pixels(*(source_bits + i*source_bpr),target_value,coeff);
						target_value = mix_2_pixels_fixed(*(source_bits + i*source_bpr),target_value,(uint32)(32768*coeff));
						coeff -= coeff_diff;
					}
					*target_bits++ = target_value;
					source_bits++;
				}
				source_bits -= source_bpr;
				accumulation += diff;			
				
			}	
		}
		else {
			for (int32 y=0;y<=bottom;y++) {
				for (int32 x=0;x<target_bpr;x++) {
					// Just copy it straight
					*target_bits++ = *source_bits++;		
				}
				
			}		
		}		
		
		if (intermediate_bitmap != original)
			delete intermediate_bitmap;
	
		return new_bitmap;
	}
	else 
		return intermediate_bitmap;
}

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

