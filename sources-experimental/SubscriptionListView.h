#ifndef _SubscriptionListView_H_
#define _SubscriptionListView_H_

#include <ColumnListView.h>
#include "SubscriptionColumn.h"

#define 	CHANNEL_SELECTED			'invo'

class GroupItem;

class SubscriptionListView : public BColumnListView
{
	public:
			 			SubscriptionListView(BRect r);
				void 	FrameResized(float w ,float h);
				void	SelectionChanged();
			    bool 	InitiateDrag(BPoint point,  bool wasSelected);
				void	MessageDropped(BMessage*, BPoint point);
				
				void	AddRow(BRow*, BRow *parent = 0);
				void	AddRow(BRow*, int32 index, BRow *parent = 0);
				
				void	AddGroup(GroupItem* );
				//TODO: void AddGroup(GroupItem* group, GroupItem* parent);
				
								
				void				SetItemSize(SubscriptionSize size); //BIG or SMALL
				SubscriptionSize	ItemSize();
				
				//Better drag support
				
				virtual void	MouseMoved(BPoint point,uint32 transit,const BMessage* message);
				
				
				
	private:
	
				void		ChangeSize(BRow* parent);
				void		DroppedSubscription(BMessage*, BPoint point);
				void		DroppedGroup(BMessage*, BPoint point);
	
				BColumn*				podcast;
				SubscriptionSize		fItemSize;
};

#endif

//.
