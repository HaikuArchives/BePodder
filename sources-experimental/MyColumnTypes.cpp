#include "MyColumnTypes.h"
#include "ColumnTypes.h"
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
BPositiveDateColumn::DrawField(BField* field,
												 BRect rect,
												 BView* parent)
{
		time_t value=(int)((BDateField*)field)->UnixTime();
		if(value<=0) return;
		BDateColumn::DrawField(field,rect,parent);
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

BMyBitmapColumn::BMyBitmapColumn		(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align):
												 BTitledColumn(title, width, minWidth, maxWidth, align)
												 {}

int BMyBitmapColumn::CompareFields(BField *field1, BField *field2){
		return (((BBitmapField*)field1)->Bitmap() - ((BBitmapField*)field2)->Bitmap());
}
					
void												 
BMyBitmapColumn::DrawField(BField* field,
												 BRect rect,
												 BView* parent){
												 	
												 	
												 	parent->PushState();	
												 								 
												 		BBitmapField *bitmapField = static_cast<BBitmapField *>(field);
														const BBitmap *bitmap = bitmapField->Bitmap();
													
														if (bitmap != NULL)
														{
															float	x = 0.0;
															float	y;
															BRect	r = bitmap->Bounds();
													
															y = rect.top; //rect.top + ((rect.Height() - r.Height()) / 2);
													
															/*switch (Alignment())
															{
																case B_ALIGN_LEFT:
																	x = rect.left + kTEXT_MARGIN;
																	break;
													
																case B_ALIGN_CENTER:
																	x = rect.left + ((rect.Width() - r.Width()) / 2);
																	break;
													
																case B_ALIGN_RIGHT:
																	x = rect.right - kTEXT_MARGIN - r.Width();
																	break;
															}*/
															x = rect.left;
															parent->SetDrawingMode(B_OP_ALPHA);
															parent->DrawBitmap(bitmap, BPoint(x,y));
															parent->SetDrawingMode(B_OP_OVER);
														}
												 	parent->PopState();
												 }
		

