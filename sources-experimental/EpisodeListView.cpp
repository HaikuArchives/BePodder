#include "EpisodeListView.h"
#include "SubscriptionColumn.h"

#include "MyColumnTypes.h"
#include <Window.h>
#include "BPLocale.h"
#include <ControlLook.h>
#include <ScrollBar.h>
#include <Box.h>
#include "EpisodeListItem.h"
#include "FileStatusColumn.h"
#include "Colors.h"
#include "ImageButton.h"

class StatusView :  public BView {
		 public:	
							StatusView(BRect r);
				void		SetCount(int32);
		private:
				BStringView*		fCounter;
};

StatusView::StatusView(BRect r):BView(r,NULL,B_FOLLOW_BOTTOM | B_FOLLOW_LEFT,B_WILL_DRAW){
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BRect rect(Bounds());
	rect.InsetBy(2,2);
	fCounter = new BStringView(rect,"","");
	fCounter->SetFontSize(10);
	BFont	font;
	fCounter->GetFont(&font);
	font_height fh;
	font.GetHeight(&fh);
	fCounter->ResizeBy(-10,fh.descent/2);
	fCounter->MoveBy(10,fh.descent/2);
	fCounter->SetViewColor( B_TRANSPARENT_COLOR );
	AddChild(fCounter);
}

void
StatusView::SetCount(int32 count){
	BString text;
	text << count;
	fCounter->SetText(text.String());
}

EpisodeListView::EpisodeListView(BRect r):
BColumnListView(r,"EpisodeListView",B_FOLLOW_ALL, B_WILL_DRAW|B_NAVIGABLE,B_PLAIN_BORDER,true)
{
	BColumn *icon = new BMyBitmapColumn(_T("Icon"),16,16,16);
	BColumn *title = new BStringColumn(_T("Title"),140,10,500,B_TRUNCATE_MIDDLE,B_ALIGN_LEFT);
	BColumn *date  = new BPositiveDateColumn(_T("Date"),70,10,150,B_ALIGN_LEFT);
	BColumn *size  = new BPositiveSizeColumn(_T("Size"),80,10,150,B_ALIGN_LEFT);
	
	int index = 0;
	AddColumn(icon,index++);
	AddColumn(title,index++);
	AddColumn(date,index++);
	AddColumn(size,index++);
	AddColumn(new FileStatusColumn(_T("Status"),200,110,400,0,B_ALIGN_LEFT),index++);
	
	SetColumnFlags((column_flags)(B_ALLOW_COLUMN_REMOVE|B_ALLOW_COLUMN_RESIZE|B_ALLOW_COLUMN_POPUP|B_ALLOW_COLUMN_MOVE));
	SetSelectionMode(B_MULTIPLE_SELECTION_LIST);
	SetSortingEnabled(true);
	SetSortColumn(date,false,false);
	AddStatusView(fStatusView=new StatusView(BRect(0, 0, 100,B_H_SCROLL_BAR_HEIGHT-1)));
}

void				
EpisodeListView::SelectionChanged(){
	uint32 buttons = 0;
	
	BMessage *msg = Window()->CurrentMessage();
	
	if(msg)	//don't remove.
		msg->FindInt32("buttons", (int32 *)&buttons) ;
	
	if(SelectionMessage())		
		SelectionMessage()->ReplaceInt32("buttons",buttons);

//	EpisodeListItem* sel = (EpisodeListItem*)CurrentSelection();
	
//	SelectionMessage()->RemoveName("entry_ref");
//	SelectionMessage()->AddRef("entry_ref",&sel->fRef);
					
	BColumnListView::SelectionChanged();
}


void				
EpisodeListView::AddRow(BRow* row, BRow *parent){
	BColumnListView::AddRow(row,parent);
	ExpandOrCollapse(row, true);
	UpdateCount();
}
void
EpisodeListView::RemoveRow(BRow* row){
	BColumnListView::RemoveRow(row);
	UpdateCount();
}
void				
EpisodeListView::AddRow(BRow* row, int32 index, BRow *parent){
	BColumnListView::AddRow(row,index,parent);
	ExpandOrCollapse(row, true);
	UpdateCount();
}

void
EpisodeListView::UpdateCount(){
	fStatusView->SetCount(CountRows());
}

				
//--
