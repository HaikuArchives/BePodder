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

				//void				DrawBackground(BRect _rect,BView* _target_view,bool _is_selected,bool _is_focus );
				
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
		
 		 			SubscriptionField*	field;
 		 			BString	fUrl; //?
 		 			BString	fWeb;
 					bool	isNew;
 						
 						
 };

#endif
