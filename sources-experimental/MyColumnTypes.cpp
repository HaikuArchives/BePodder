#include "MyColumnTypes.h"
#include "ColumnTypes.h"
#include <DateFormat.h>
#include <DateTimeFormat.h>
#include <stdio.h>
#include <View.h>

#include "Utils.h"

#define kTEXT_MARGIN	8

//=====================================================================

BPositiveIntegerColumn::BPositiveIntegerColumn(const char* title, float width, float minWidth,
							   float maxWidth, alignment align)
	:BTitledColumn(title, width, minWidth, maxWidth, align)
{
		SetWantsEvents(true);
}


//--------------------------------------------------------------------

void BPositiveIntegerColumn::DrawField(BField *field, BRect rect, BView* parent)
{
	char	formatted[256];
	float	width = rect.Width() - (2 * kTEXT_MARGIN);
	BString	string;

	int value=(int)((BIntegerField*)field)->Value();
	
	if(value<=0) return;
	
	sprintf(formatted, "%d", value );

	string = formatted;
	parent->TruncateString(&string, B_TRUNCATE_MIDDLE, width + 2);
	DrawString(string.String(), parent, rect);
}


//--------------------------------------------------------------------

int BPositiveIntegerColumn::CompareFields(BField *field1, BField *field2)
{
	return (((BIntegerField*)field1)->Value() - ((BIntegerField*)field2)->Value());
}




 void
 BPositiveSizeColumn::MouseMoved(BColumnListView *parent, BRow *row, BField *field,
										BRect field_rect, BPoint point, uint32 buttons, int32 code, const BMessage *msg ){
										

										}



//--------------------------------------------------------------------

BPositiveSizeColumn::BPositiveSizeColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align):
								BSizeColumn(title,width,minWidth,maxWidth,align){}
								
void
BPositiveSizeColumn::DrawField(BField* field,
												 BRect rect,
												 BView* parent)
{
		off_t value=(int)((BSizeField*)field)->Size();
		if(value<=0) return;
		BSizeColumn::DrawField(field,rect,parent);
}

//--------------------------------------------------------------------

BPositiveDateColumn::BPositiveDateColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align ):
								BDateColumn(title,width,minWidth,maxWidth,align){}
								
void
BPositiveDateColumn::DrawField(BField* _field,
												 BRect rect,
												 BView* parent)
{
	time_t value=(int)((BDateField*)_field)->UnixTime();
	if(value<=0) return;

	float width = rect.Width() - (2 * kTEXT_MARGIN);
	BDateField* field = (BDateField*)_field;

	if (field->Width() != rect.Width()) {

		float resultWidth = rect.Width() + 1;

		time_t timeValue = (time_t)value;

		// Find the longest possible format that will fit the available space
		struct {
			BDateFormatStyle dateStyle;
			BTimeFormatStyle timeStyle;
		} formats[] = {
			{ B_LONG_DATE_FORMAT, B_MEDIUM_TIME_FORMAT },
			{ B_LONG_DATE_FORMAT, B_SHORT_TIME_FORMAT },
			{ B_MEDIUM_DATE_FORMAT, B_SHORT_TIME_FORMAT },
			{ B_SHORT_DATE_FORMAT, B_SHORT_TIME_FORMAT },
		};

		BString dateString;
		BDateTimeFormat formatter;
		for (unsigned int i = 0; i < B_COUNT_OF(formats); ++i) {
			if (formatter.Format(dateString, timeValue, formats[i].dateStyle,
					formats[i].timeStyle) == B_OK) {
				resultWidth = parent->StringWidth(dateString.String(), dateString.Length());
				if (resultWidth <= width) {
					// Found a format that fits the available space, stop searching
					break;
				}
			}
		}
		if (resultWidth > width) {
			BDateFormat dateFormatter;
			if (dateFormatter.Format(dateString, timeValue, B_SHORT_DATE_FORMAT) == B_OK) {
				resultWidth = parent->StringWidth(dateString.String(), dateString.Length());
			}
		}
		if (resultWidth > width) {
			BString out_string(dateString);
			parent->TruncateString(&out_string, B_TRUNCATE_MIDDLE, width + 2);
			dateString = out_string;
		}
		field->SetClippedString(dateString);
		field->SetWidth(resultWidth);
	}
	DrawString(field->ClippedString(), parent, rect);
}

												 
//--------------------------------------------------------------------

BPositiveGraphColumn ::BPositiveGraphColumn(const char* name,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align):
					BIntegerColumn(name,width,minWidth,maxWidth,align)	
{
		fBar=LoadIcon("fullbar.png");
		fBack=LoadIcon("graybar.png");
}

void
BPositiveGraphColumn ::DrawField(BField* field,
												 BRect rect,
												 BView* parent)
{
	int number=(int)((BIntegerField*)field)->Value();
	if(number<=0) return;
	
	parent->PushState();	
	
	if (number > 100)
		number = 100;
	else if (number < 0)
		number = 0;

	BRect graphRect(rect);
	graphRect.top += 2;
	graphRect.bottom = graphRect.top + 11;
	graphRect.right = graphRect.left + 99;
	
	parent->DrawBitmap(fBack,graphRect); //BPoint(rect.right,rect.top));
	

	if (number > 0) {
		BRect sourceRect(fBar->Bounds());
		sourceRect.right =  sourceRect.left + number;
		
		BRect destRect(graphRect);
		destRect.right = destRect.left + number; 
		parent->DrawBitmap(fBar,sourceRect,destRect); //sourceRect,destRect);
	}

	char numstr[256];
	sprintf(numstr, "%d%%", number);

	float width = be_plain_font->StringWidth(numstr);
	parent->MovePenTo(rect.left + rect.Width() / 2 - width / 2, rect.bottom - FontHeight());
	parent->DrawString(numstr);
	
	
	//BGraphColumn::DrawField(field,rect,parent);
	parent->PopState();
}
