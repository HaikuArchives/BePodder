#ifndef  _FileStatusColumn_H
#define _FileStatusColumn_H

#include <ColumnListView.h>
#include <ColumnTypes.h>
#include <String.h>
#include <Font.h>
#include <Bitmap.h>
#include "FileStatus.h"


//=====================================================================
// Field and column classes a FileStatus.

class FileStatusField : public BField
{
	public:
									FileStatusField		(FileStatus status);

				void				SetFileStatus(FileStatus file);
				void				SetFilePercentage(int per,float speed = 0);
				
				void				SetString			(const char* string);
				const char*	String				() const;
				void				SetClippedString	(const char* string);
				const char*	ClippedString		();
				void				SetWidth			(float);
				float				Width				();
				FileStatus		GetFileStatus(){ return fStatus;}
				int					GetPercentage(){return fPercentage;}
				//bool				HasChanged() { return fChanged; }
				//void				SetChanged(bool val) { fChanged = val;}
	private:

		float				fWidth;
		BString				fString;
		BString				fClippedString;
		BString				fOriginalStatus;
		FileStatus			fStatus;
		int					fPercentage;

};

class FileStatusColumn : public BTitledColumn
{
	public:
							FileStatusColumn		(const char *title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 uint32 truncate,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1,
												 BField* field2);
		virtual	bool		AcceptsField        (const BField* field) const;

	private:
		void		DrawBar(BView* parent,BRect rect,int perc);
		uint32		fTruncate;
		
};

#endif
