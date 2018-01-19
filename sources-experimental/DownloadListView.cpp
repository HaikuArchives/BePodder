#include "DownloadListView.h"
#include "SubscriptionColumn.h"

#include "MyColumnTypes.h"
#include <Window.h>
#include "BPLocale.h"
#include <ScrollBar.h>
#include <Box.h>

#include "FileStatusColumn.h"
#include "DownloadListItem.h"

DownloadListView::DownloadListView(BRect r):
BColumnListView(r,"DownloadListView",B_FOLLOW_ALL, B_WILL_DRAW|B_NAVIGABLE,B_FANCY_BORDER,true)
{
	BColumn *icon = new BBitmapColumn(_T("Icon"),16,16,16,B_ALIGN_CENTER);
	BColumn *channel = new BStringColumn(_T("Subscription"),140,10,500,5,B_ALIGN_LEFT);
	BColumn *title = new BStringColumn(_T("Title"),140,10,500,5,B_ALIGN_LEFT);
	BColumn *date  = new BPositiveDateColumn(_T("Date"),70,10,250,B_ALIGN_LEFT);
	BColumn *size  = new BPositiveSizeColumn(_T("Size"),80,10,150,B_ALIGN_LEFT);
	
	int index = 0;
	AddColumn(icon,index++);
	AddColumn(channel,index++);
	AddColumn(title,index++);
	AddColumn(date,index++);
	AddColumn(size,index++);
	AddColumn(new FileStatusColumn(_T("Status"),
														200,110,400,0,B_ALIGN_LEFT),
						index++);
	//SetColumnFlags(B_ALLOW_COLUMN_RESIZE);
	SetColumnFlags((column_flags)(B_ALLOW_COLUMN_REMOVE|B_ALLOW_COLUMN_RESIZE|B_ALLOW_COLUMN_POPUP|B_ALLOW_COLUMN_MOVE));
	SetSelectionMode(B_SINGLE_SELECTION_LIST);
	SetSortingEnabled(true);
	SetSortColumn(date,false,false);
	
	BView* sview = new BView(BRect(0,0,100,B_H_SCROLL_BAR_HEIGHT-1),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	sview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	 
	fCounter = new BStringView(BRect(2,1,98,B_H_SCROLL_BAR_HEIGHT-2),"","");
	fCounter->SetFontSize(10);
	fCounter->SetViewColor( ui_color(B_PANEL_BACKGROUND_COLOR) );
	
	sview->AddChild(fCounter);
	AddStatusView(sview);
	
	const rgb_color greyBox = {255,234,130 }; 
	SetColor(B_COLOR_SELECTION,greyBox);
	SetColor(B_COLOR_SELECTION_TEXT,Color(B_COLOR_TEXT));
	
	const rgb_color white={255,255,255,255};
    SetColor(B_COLOR_BACKGROUND,white);
               
	
	SetSortingEnabled(true);
	ClearSortColumns();
	//SetSortColumn(ColumnAt(index),true,true);
	
	
	//messages
	
	BMessage* selected=new BMessage(DOWNLOAD_SELECTED);
	selected->AddInt32("buttons",0);
	
	SetSelectionMessage(selected);
	
	SetInvocationMessage(new BMessage(DOWNLOAD_INVOKED));
}

void				
DownloadListView::SelectionChanged(){
	uint32 buttons = 0;
	
	BMessage *msg = Window()->CurrentMessage();
	
	if(msg)	//don't remove.
		msg->FindInt32("buttons", (int32 *)&buttons) ;
		
		
	DownloadListItem* dli =(DownloadListItem*) CurrentSelection();
	if(!dli) return;	
	
	if(SelectionMessage()){
	
		SelectionMessage()->ReplaceInt32("buttons",buttons);
		SelectionMessage()->AddRef("entry_ref",&dli->fRef);
					
	}
	BColumnListView::SelectionChanged();
}

void
DownloadListView::ResetSelectionMessage(){

	if(SelectionMessage()){
	
		SelectionMessage()->ReplaceInt32("buttons",0);
		SelectionMessage()->RemoveName("entry_ref");
					
	}
}

void				
DownloadListView::AddRow(BRow* row, BRow *parent){
	BColumnListView::AddRow(row,parent);
	UpdateCount();
}
void				
DownloadListView::AddRow(BRow* row, int32 index, BRow *parent){
	BColumnListView::AddRow(row,index,parent);
	UpdateCount();
}

void
DownloadListView::UpdateCount(){
	BString text;
	text << CountRows();
	fCounter->SetText(text.String());
}

				
//--
