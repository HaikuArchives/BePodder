#include "SubscriptionColumn.h"
#include <Catalog.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <StringFormat.h>
#include <Window.h>
#include "Colors.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SubscriptionColumn"


#define kTEXT_MARGIN	8

SubscriptionColumn::SubscriptionColumn(const char* title, float width, float minWidth,
							 float maxWidth, uint32 trunc, alignment align)
	:BStringColumn(title, width, minWidth, maxWidth, align),
	fTruncate(trunc)
{
		SetWantsEvents(true);
		lastDest = NULL;
}

//--------------------------------------------------------------------

void SubscriptionColumn::DrawField(BField* _field, BRect rect, BView* parent){

	SubscriptionField*	field = dynamic_cast<SubscriptionField*>(_field);

	if(field){

		switch(field->Size()){
			case BIG:
				DrawBigField(field,rect,parent);
			break;
			case SMALL:
				DrawSmallField(field,rect,parent);
			break;
		}
	}
	else
		BStringColumn::DrawField(_field,rect,parent);

}

void
SubscriptionColumn::DrawSmallField(SubscriptionField* field, BRect rect, BView* parent)
{
	parent->PushState();
	rect.left += kTEXT_MARGIN;

	float			width = rect.Width() - (2 * kTEXT_MARGIN);

	BString		out_string(field->String());




	if(	field->GetNewCount() >0)
		out_string << " (" <<	field->GetNewCount() << ")";
	else
		out_string << " "; //really don't like maybe we should find a better solution? to avoid the (1) bug

	if (width != field->Width()  || out_string.Compare(field->String()) !=0 )
	{
		parent->TruncateString(&out_string, fTruncate, width + 2);
		field->SetClippedString(out_string.String());
		field->SetWidth(width);
	}

	SetFileStatusColor(field->GetFileStatus(),parent);

	DrawString(field->ClippedString(), parent, rect);
	parent->PopState();
}

void
SubscriptionColumn::DrawBigField(SubscriptionField* field, BRect rect, BView* parent)
{
	//SubscriptionField*	field = static_cast<SubscriptionField*>(_field);

	const BBitmap *bitmap = field->GetBitmap();

	parent->PushState();

	if (bitmap != NULL)
	{
		float	x = 0.0;
		float	y;
		BRect	r = bitmap->Bounds();

		y = rect.top + ((rect.Height() - r.Height()) / 2) - 1;

		switch (Alignment())
		{
			case B_ALIGN_LEFT:
				x = rect.left ; // + kTEXT_MARGIN;
				break;

			case B_ALIGN_CENTER:
				x = rect.left + ((rect.Width() - r.Width()) / 2);
				break;

			case B_ALIGN_RIGHT:
				x = rect.right - kTEXT_MARGIN - r.Width();
				break;
			case B_ALIGN_HORIZONTAL_UNSET:
			case B_ALIGN_USE_FULL_WIDTH:
				break;
		}
		parent->SetDrawingMode(B_OP_ALPHA);
		parent->DrawBitmap(bitmap, BPoint(x, y));
		parent->SetDrawingMode(B_OP_OVER);
	}

	rect.left += 45.0; //55.0;

	float width = rect.Width() - (2 * kTEXT_MARGIN);
	parent->SetFont(be_bold_font);
	if (width != field->Width())
	{
		BString		out_string(field->String());

		parent->TruncateString(&out_string, fTruncate, width + 2);
		field->SetClippedString(out_string.String());
		field->SetWidth(width);
	}

	float hei=rect.Height() /2.0;
	hei -= 3;
	rect.top +=3;

	rect.bottom = rect.top + hei; //+ 30;  //19
	DrawString(field->ClippedString(), parent, rect);

	//debug: 	parent->StrokeRect(rect);
	rect.top = rect.bottom;
	rect.bottom = rect.top + hei; //19;   //19
	//parent->SetHighColor(0,0,0);      //(125,125,125)

	DrawString("", parent, rect);
	//debug: 	parent->StrokeRect(rect);

	parent->SetFont(be_plain_font);
	// we should show the message "no new items"?
	if(field->GetNewCount()>0 && (field->GetFileStatus() == DOWNLOADED
		|| field->GetFileStatus() == STOPPED))
	{
		BString itemsNumber;
		static BStringFormat formatItems(B_TRANSLATE("{0, plural,"
		"=0{no new items}"
		"=1{1 new item}"
		"other{# new items}}" ));
		formatItems.Format(itemsNumber, field->GetNewCount());
		parent->DrawString(itemsNumber.String());
	}
	else
		DrawStatus(field->GetFileStatus(),parent,field->GetFilePercentage());

	parent->PopState();
}

void
SubscriptionColumn::DrawStatus(FileStatus status,BView* parent,int32 perc){

	SetFileStatusColor(status,parent);

	switch(status){

		case ERROR:
			parent->DrawString(B_TRANSLATE("error"));
		break;
		case NOT_DOWNLOADED:
			parent->DrawString(B_TRANSLATE("not down"));
		break;
		case ENQUEQUED:
			parent->DrawString(B_TRANSLATE("enqueued"));
		break;
		case DOWNLOADING:
			parent->DrawString(B_TRANSLATE("downloading"));
			if(perc>=0 && perc<=100){
			 BString sp(" ");
			 sp << perc << "%";
			 parent->DrawString(sp.String());
			}
		break;
		case DOWNLOADED:
		break;
		case STOPPED:
		break;
		case CONNECTING:
		 	parent->DrawString(B_TRANSLATE("connecting"));
		break;
		case NOT_FOUND:
			parent->DrawString(B_TRANSLATE("not found"));
		break;
		case CANT_CONNECT:
			parent->DrawString(B_TRANSLATE("can't connect"));
		break;
		case BAD_FORMAT:
			parent->DrawString(B_TRANSLATE("invalid format"));
		break;
		default:
		break;
	}



}

void
SubscriptionColumn::SetFileStatusColor(FileStatus status,BView* parent){
	switch(status){

		case ERROR:
			parent->SetHighColor(230,0,0);
		//	parent->SetLowColor(125,0,0);

		break;
		case NOT_DOWNLOADED:

		break;
		case ENQUEQUED:
			parent->SetHighColor(20,20,220);
		break;
		case DOWNLOADING:
			parent->SetHighColor(0,99,12);   //(0,255,0)
			//parent->SetLowColor(0,49,12);     //(0.125.0)

		break;
		case DOWNLOADED:

		break;
		case STOPPED:

		break;
		case CONNECTING:
		 	parent->SetHighColor(83,118,75); //57,93,0);
			//parent->SetLowColor(57,93,0);     //(0.125.0)

		break;
		case NOT_FOUND:
			parent->SetHighColor(230,0,0);
		//	parent->SetLowColor(125,0,0);

		break;
		case CANT_CONNECT:
			parent->SetHighColor(230,0,0);
		//	parent->SetLowColor(125,0,0);

		break;
		case BAD_FORMAT:
			parent->SetHighColor(230,0,0);
		//	parent->SetLowColor(125,0,0);

		break;
		default:
		break;
	}
//	parent->SetLowColor(parent->HighColor());
}

//--------------------------------------------------------------------

int SubscriptionColumn::CompareFields(BField* field1, BField* field2) {
	int32 index1 = ((PositionableField*)field1)->GetPreferredPosition();
	int32 index2 = ((PositionableField*)field2)->GetPreferredPosition();
	return (  index1 - index2 );
}


//--------------------------------------------------------------------

bool SubscriptionColumn::AcceptsField(const BField *field) const {
	return static_cast<bool>(dynamic_cast<const PositionableField*>(field));
}


void
SubscriptionColumn::MouseMoved(BColumnListView *parent, BRow *row, BField *field,
					               BRect field_rect, BPoint point, uint32 buttons, int32 code){

	if(parent && parent->Window()){
		BMessage* msg = parent->Window()->CurrentMessage();
		int32 what;
		if(msg && msg->FindInt32("_msg_data_",&what)==B_OK) {



			BRect re;
			BRow* dest=parent->RowAt(point);
			parent->GetRowRect(dest,&re);

			/*
				casi possibili di update vecchio
				a) !=NULL
				b) != nuovo
				c) == nuovo E !contine
			*/

			if(lastDest && (  ( lastDest != dest ) || ( dest && lastDest == dest && !re.Contains(point) )  ))
							parent->UpdateRow(lastDest);


			if(dest && dest != row && re.Contains(point) && lastDest != dest ){
					parent->ScrollView()->PushState();
					parent->ScrollView()->SetHighColor(White);
					parent->ScrollView()->SetHighColor(200,0,0,125);
					parent->ScrollView()->SetDrawingMode(B_OP_ALPHA);
					parent->ScrollView()->FillRect(re);
					parent->ScrollView()->PopState();
			}

			if(re.Contains(point))
				lastDest=dest;
			else
				lastDest = NULL;
		}
	}
}
