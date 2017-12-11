#ifndef	SubscriptionColumn_H
#define SubscriptionColumn_H

#include <ColumnTypes.h>
#include "FileStatus.h"

enum SubscriptionSize {
						BIG,
						SMALL
					};
					

class PositionableField : public BStringField {

		public:
				
				PositionableField(const char* txt):BStringField(txt){
					SetPreferredPosition(-1);
					SetSize(SMALL);
				};
				
				void				SetSize(SubscriptionSize size) { fSize=size; }
				SubscriptionSize	Size(){ return fSize;}
								
				void		SetPreferredPosition(int c) { fPosition=c;}
				int 		GetPreferredPosition() { return fPosition; }
				
				
				
		private:
				
				int						fPosition;
				SubscriptionSize		fSize;

};

class SubscriptionField : public PositionableField {

		public:
				
				SubscriptionField(const char* txt,int32):PositionableField(txt){
					SetNewCount(0);
					SetBitmapPointer(NULL);
					fMyBitmap =NULL;
					fPercentage = 0;
					SetFileStatus(DOWNLOADED);
					SetSize(BIG);
				};
				
		
				void		SetBitmapPointer(BBitmap* bit){ fBitmap=bit; }
				void		SetBitmapPersonal(BBitmap* bit){
				 				if(fMyBitmap) delete fMyBitmap;
				 				fMyBitmap=bit;
				 				SetBitmapPointer(bit);
							}
				
				FileStatus	GetFileStatus(){ return fStatus;};
				void			SetFileStatus(FileStatus status){ fStatus = status; };
				void			SetFilePercentage(int perc){ fPercentage = perc; }
				int32			GetFilePercentage(){ return fPercentage; }
				
				void		SetNewCount(int32 count){ fNewCount=count;}
				void		AddNewCount(int32 add){ fNewCount +=add; }
				int32		GetNewCount(){ return fNewCount; }
				
				BBitmap*	GetBitmap() { return fBitmap; }
		private:
				BBitmap*	fBitmap;
				BBitmap*	fMyBitmap;
				FileStatus	fStatus;
				int32			fNewCount;
				int32			fPercentage;
				//int				fPosition;
				//SubscriptionSize		fSize;

};

class SubscriptionColumn : public BStringColumn {

	public:
							SubscriptionColumn		(const char *title,
												 float width,
												 float maxWidth,
												 float minWidth,
												 uint32 truncate,
												 alignment align = B_ALIGN_LEFT);
		 void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		void		DrawBigField			(SubscriptionField* field,
												 BRect rect,
												 BView* parent);
		void		DrawSmallField			(SubscriptionField* field,
												 BRect rect,
												 BView* parent);
												 
		 virtual int			CompareFields		(BField* field1,
												 BField* field2);
			bool		AcceptsField        (const BField* field) const;
	
		
				void		DrawStatus(FileStatus status,BView* parent,int32 perc);
				void		SetFileStatusColor(FileStatus status,BView* parent);
		
		
			void		MouseMoved(BColumnListView *parent, BRow *row, BField *field,
					               BRect field_rect, BPoint point, uint32 buttons, int32 code);
	private:
		uint32					fTruncate;
		BRow*					lastDest;
};
#endif
