/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _EpisodeListItem_H
#define _EpisodeListItem_H

#include <ColumnListView.h>

#include "ElementListener.h"

#include "FileStatusColumn.h"

#include "RssItem.h"
#include <Entry.h>

class EpisodeListItem : public BRow, public ElementListener
{
	public:
					 		  EpisodeListItem();
				   	 		~EpisodeListItem();

	    void 				KeyModified(int32 key, void* data,ssize_t size);
		void				DrawBackground(	BRect _rect, BView* _target_view, bool _is_selected, bool _is_focus );
		
		bool	IsNew(){ return isNew; }
		
		off_t				GetEnclosureSize(){ return sizefield->Size(); };				 
		const char*  GetTitle(){	return field->String(); };
		time_t		    GetDate(){	return ((BDateField*)GetField(2))->UnixTime(); };	
				 
		entry_ref	fRef;
				
		private:
					void	SetIsNew(bool set){ isNew = set; }								
		
					BBitmapField*		iconfield;
 		 			BStringField*			field;
 		 			BSizeField*				sizefield;
 		 			FileStatusField*	fieldStaus;

					bool	isNew;
					float	fLatestSpeed;
};

#endif
