/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _SubscriptionListItem_H
#define _SubscriptionListItem_H

#include "ColumnListView.h"
#include "ColumnTypes.h"

#include "ElementListener.h"

#include "SubscriptionColumn.h"

#include "RssChannel.h"

#include <Entry.h>


class SubscriptionListItem : public BRow, public ElementListener
{
	public:
			
					
					 SubscriptionListItem(entry_ref		fRef);
				   	 ~SubscriptionListItem();
	
								BString GetURL(){ return fUrl;}
								BString	GetWeblink(){ return fWeb;}
					void		SetFileStatus(FileStatus c){field->SetFileStatus(c);}
					FileStatus		GetFileStatus(){return field->GetFileStatus();}
					
					BString		GetTitle(){return field->String();}				
			   		void		KeyModified(int32 key, void* data,ssize_t);
				
				void		SetNewCount(int32 count){ field->SetNewCount(count);}
				void		AddNewCount(int32 add){ field->AddNewCount(add);}
				int32		GetNewCount(){ return field->GetNewCount(); }
				
				void		SetPreferredPosition(int32 index){
							field->SetPreferredPosition(index);
				}
				
				void		SetFilePercentage(int32 per){ field->SetFilePercentage(per);}
				
				void				SetSize(SubscriptionSize size);
				
				bool				HasOwnImage();

				//don't like this:
				RssChannel	fChannel;

				void				DrawBackground(BRect _rect,BView* _target_view,bool _is_selected,bool _is_focus );
				
				bool				IsNew(){ return isNew; }
				void				SetIsNew(bool set){ isNew = set; }
				
				entry_ref		fRef;
				
				entry_ref		lastSelected; //<---- ? remove and add as list.
				
				BMessage*	extraSetting;		//TODO: per channel setttings
				BMessage*	columnsState;		//columns states
				//TODO:		list of selected entry_ref.
				
				enum StatusFilter	{
						  FILTER_NONE 				= 0,
						  FILTER_NEW				= 1,
						  FILTER_DOWNLOADED = 2
				};
				
				
				StatusFilter			fStatusFilter;	//maybe can become an attribute?
												
		private:
		
					BBitmap*	rescale_bitmap( BBitmap * src, int32 width, int32 height );
 		 			SubscriptionField*	field;
 		 			BString	fUrl; //?
 		 			BString	fWeb;
 		 			
 		 			inline uint32 mix_2_pixels_fixed(uint32 p1,uint32 p2,uint32 c)
					{
							uint32 inv_c = 32768 - c;
	
							return 	((((((p1 >> 24) &0xFF) * c + ((p2 >> 24)&0xFF) * inv_c)>>15)<<24) & 0xFF000000) |
									((((((p1 >> 16)&0xFF) * c + ((p2 >> 16)&0xFF) * inv_c)>>15)<<16) & 0x00FF0000) |
									((((((p1 >> 8)&0xFF) * c + ((p2 >> 8)&0xFF) * inv_c)>>15)<<8) & 0x0000FF00) |
									(((((p1&0xFF) * c + (p2&0xFF) * inv_c)>>15)) & 0x000000FF);				
					}
 					
 					bool	isNew;
 						
 						
 };

#endif
