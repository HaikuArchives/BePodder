#ifndef _MyColumnType_H
#define _MyColumnType_H

#include "ColumnListView.h"
#include "ColumnTypes.h"
#include <String.h>
#include <Font.h>
#include <Bitmap.h>


//--------------------------------------------------------------------
// DrawString only if the value is positive.

class BPositiveIntegerColumn : public BTitledColumn
{
	public:
						BPositiveIntegerColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		 		 void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		 	 int			CompareFields		(BField* field1,
												 BField* field2);
												 
												 
									
};

class BPositiveSizeColumn : public BSizeColumn
{
	public:
							BPositiveSizeColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		  void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
												 
			//test code:
			void				MouseMoved(BColumnListView *parent, BRow *row, BField *field,BRect field_rect, BPoint point, uint32 buttons, int32 code, const BMessage *msg = NULL);
	
};

class BPositiveDateColumn : public BDateColumn
{
	public:
							BPositiveDateColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		  void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
};

class BPositiveGraphColumn : public BIntegerColumn
{
	public:
							BPositiveGraphColumn(const char* name,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
				 void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
												 
	private:
				BBitmap*		fBar;
				BBitmap*		fBack;
};


class BMyBitmapColumn : public BTitledColumn
{
	public:
							BMyBitmapColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		 		 void		DrawField		(BField* field,
												 BRect rect,
												 BView* parent);
												 
				int			CompareFields(BField* field1,
												 BField* field2);
		
};


#endif

