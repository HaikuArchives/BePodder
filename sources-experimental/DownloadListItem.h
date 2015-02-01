/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef _DownloadListItem_H
#define _DownloadListItem_H

#include "ColumnListView.h"
#include "ColumnTypes.h"

#include "ElementListener.h"

#include "FileStatusColumn.h"

#include <Entry.h>
#include "RssItem.h"

class DownloadListView;
class EpisodeListItem;
class	SubscriptionListItem;

class DownloadListItem : public BRow, public ElementListener
{
	public:
					 DownloadListItem(SubscriptionListItem* channel);
				   	 ~DownloadListItem();
	
		
		void KeyModified(int32 key, void* data,ssize_t size);
		

		entry_ref	fRef;
		RssItem fRss;	
		
		
					void		LinkToEpisode(EpisodeListItem* item);
					void		Unlink();
					
					float		GetLastSpeed();
					int32	GetPercentage();
					
					//test
					bool		fShouldMeRemoved;
					
		private:
		
					BBitmapField*		iconfield;
 		 			BStringField*			field;
 		 			BStringField*			channel;
 		 			BSizeField*				sizefield;
 		 			FileStatusField*	fieldStaus;
 		 			EpisodeListItem*	fLinked;
 		 			
	 			
				  float	fLatestSpeed;
		
};

#endif
