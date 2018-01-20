#include "SubscriptionListView.h"
#include "SubscriptionListItem.h"
#include <Catalog.h>
#include <Window.h>
#include "Utils.h"
#include "GroupItem.h"
#include "Colors.h"

#include <InterfaceDefs.h>

#include <stdio.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SubscriptionListView"


#define DROP_CHANNEL	'drch'
#define DROP_GROUP		'drgr'

SubscriptionListView::SubscriptionListView(BRect r):
BColumnListView(r,"SubscriptionListView",B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE,B_FANCY_BORDER,true)
{

	podcast=new SubscriptionColumn(B_TRANSLATE("Subscriptions"),300,36,600,B_TRUNCATE_MIDDLE,B_ALIGN_LEFT);
	AddColumn(podcast,0);
	SetColumnFlags(B_ALLOW_COLUMN_RESIZE );
	SetSelectionMode(B_SINGLE_SELECTION_LIST);
	const rgb_color x = {238,185,0,255};    //244,203,63     244,190,23
	SetColor(B_COLOR_SELECTION,x);//!
	
     SetColor(B_COLOR_BACKGROUND,White);
               
     SetColor(B_COLOR_ROW_DIVIDER,BeBackgroundGrey);
               
               
	CurrentSelection(0);
	
	BMessage* selected=new BMessage(CHANNEL_SELECTED);
	selected->AddInt32("buttons",0);
	SetSelectionMessage(selected);
	
	
	SetSortColumn(podcast,true,true);
	SetSortingEnabled(false);
	fItemSize = BIG;
}

void
SubscriptionListView::FrameResized(float w,float h){
	BColumnListView::FrameResized(w,h);
	podcast->SetWidth(w-35);
	// GetTitleView()->Invalidate(); // TODO this method doesn't exist anymore
	Refresh();

}

void				
SubscriptionListView::SelectionChanged(){
	uint32 buttons;
	
	BMessage *msg = Window()->CurrentMessage();
	
	if(msg)
		msg->FindInt32("buttons", (int32 *)&buttons);
	
	SelectionMessage()->ReplaceInt32("buttons",buttons);
	
	BColumnListView::SelectionChanged();
}

void 
SubscriptionListView::MessageDropped(BMessage* msg, BPoint point){

	if(msg->what==DROP_CHANNEL) DroppedSubscription(msg,point);
	else
	if(msg->what==DROP_GROUP) DroppedGroup(msg,point);
	
	
}

void 
SubscriptionListView::DroppedSubscription(BMessage* msg, BPoint point){
	
		int32 from=msg->FindInt32("from");
		int32 parentFrom=msg->FindInt32("parent");
		
		BRow	* frow=	NULL;
		
		if(parentFrom!=-1){
			BRow* parent=	RowAt(parentFrom);
			if(!parent) return; //some error occured.
			frow=RowAt(from,parent);
		}
		else
			frow=RowAt(from);
		
		BRow *row=RowAt(point);
	
		if(!frow || frow==row) return;
		
		SubscriptionListItem*	sitem=dynamic_cast<SubscriptionListItem*>(row);
		
		if(row && sitem) {
				int32 to = IndexOf(row); //this is a position relative to the parent.
				
				//printf("Move from %ld to %ld\n",from,to);
				BRow* parent=NULL;
				FindParent(row,&parent,NULL);
				RemoveRow(frow);
				AddRow(frow,to,parent);
				DeselectAll();
		}
		else 
		if(row){ //dropped to a group
				RemoveRow(frow);
				AddRow(frow,row);
				DeselectAll();
		}
		else
		if(!row){ //dropped outside any other channel == add at the bottom.
			RemoveRow(frow);
			AddRow(frow,CountRows(),NULL);
		}	

		AddToSelection(frow);
}


void 
SubscriptionListView::DroppedGroup(BMessage* msg, BPoint point)
{
		int32 from=msg->FindInt32("from");
		int32 parentFrom=msg->FindInt32("parent");
		
		BRow* frow=	NULL;
		
		if(parentFrom!=-1){
			BRow* parent=	RowAt(parentFrom);
			if(!parent) return; //some error occured.
			frow=RowAt(from,parent);
		}
		else
			frow=RowAt(from);
		

	BRow *row=RowAt(point);
	if(!row) return;
	
	GroupItem*	gitem=dynamic_cast<GroupItem*>(row);
	if(gitem) 
	{
		printf("Dropped, we should move the dropped into the group '%s'\n", gitem->GroupName().String() );
		RemoveRow(frow);
		AddRow(frow,row);
		DeselectAll();
	}
}


bool
SubscriptionListView::InitiateDrag(BPoint p, bool )
{

	
	BMessage *msg = Window()->CurrentMessage();
	if(msg->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON) return false;
		
	BRow *item=CurrentSelection(NULL);
	if(!item) return false;
	
	SubscriptionListItem*	sitem=dynamic_cast<SubscriptionListItem*>(item);
	if(sitem) {	
		BMessage re(DROP_CHANNEL);
		re.AddInt32("from",IndexOf(item));
		//are we in a group?
		BRow*	parent=NULL;
		FindParent(item,&parent,NULL);
		if(parent) 
			re.AddInt32("parent",IndexOf(parent));
		else
			re.AddInt32("parent",-1);
	
		#ifdef ZETA
			BBitmap *bit=new BBitmap(*( ((SubscriptionField*)item->GetField(0))->GetBitmap() ));
		#else
			BBitmap *bit=new BBitmap( ((SubscriptionField*)item->GetField(0))->GetBitmap() );
		#endif
		DragMessage(&re,bit,B_OP_ALPHA,BPoint(bit->Bounds().Width()/2,bit->Bounds().Height()/2),this);		
		
		return true;
	}
	
	GroupItem*	gitem=dynamic_cast<GroupItem*>(item);
	if(gitem) {	
		BMessage re(DROP_GROUP);
		re.AddInt32("from",IndexOf(item));
		BRect r(0,0,20,20);
		r.OffsetTo(p.x,p.y);
		DragMessage(&re,r,this);		
		
		return true;
	}
	
	return false;
}

SubscriptionSize		
SubscriptionListView::ItemSize(){ 
	return fItemSize;
}

void	
SubscriptionListView::SetItemSize(SubscriptionSize size){

	//I hope we are locked!
	//change the size of every BRow
	
	if(	fItemSize == size ) return;
	
	fItemSize = size;
	
	BRow* selected = CurrentSelection();
	
	DeselectAll();
	
	ChangeSize(NULL);
	
	
	AddToSelection(selected);

	//redraw all!
	Refresh();
}

void
SubscriptionListView::ChangeSize(BRow* parent){
	
	int32 tot = CountRows(parent);
	for(int i=0;i<tot;i++){
		SubscriptionListItem*	item=dynamic_cast<SubscriptionListItem*>(RowAt(0,parent));
		if(item) {
			RemoveRow(item);
			AddRow(item,tot,parent);
		}
		GroupItem*	grp=dynamic_cast<GroupItem*>(RowAt(0,parent));
		if(grp) ChangeSize(grp);
	}
}

void				
SubscriptionListView::AddRow(BRow* row, BRow *par){

	SubscriptionListItem*	item=dynamic_cast<SubscriptionListItem*>(row);
	if(item)
		item->SetSize(fItemSize);
	BColumnListView::AddRow(row,par);

}

void
SubscriptionListView::AddGroup(GroupItem* group){
	AddRow((BRow*)group,(BRow*)NULL);
}



void			
SubscriptionListView::AddRow(BRow* row, int32 index, BRow *par){
	
	SubscriptionListItem*	item=dynamic_cast<SubscriptionListItem*>(row);
	if(item)
		item->SetSize(fItemSize);
	BColumnListView::AddRow(row,index,par);
}


void	
SubscriptionListView::MouseMoved(BPoint point,uint32 transit,const BMessage* message){
}

