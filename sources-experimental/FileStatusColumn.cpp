#include "FileStatusColumn.h"
#include <Catalog.h>

#include "Utils.h"
#include "stdio.h"

#define kTEXT_MARGIN	8
#define kSPACE_TEXT		0


const int64 	kKB_SIZE				= 1024;
const int64 	kMB_SIZE				= 1048576;
const int64 	kGB_SIZE				= 1073741824;
const int64 	kTB_SIZE				= kGB_SIZE * kKB_SIZE;

		static BBitmap*			fBar1 = NULL; 
		static BBitmap*			fBar2 = NULL;
		static BBitmap*			fBack = NULL;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FileStatusColumn"

//=====================================================================

FileStatusField::FileStatusField(FileStatus status)
	:fWidth(0),
	fString(""),
	fClippedString("")
{
	SetFileStatus(status);
	SetFilePercentage(0);
	SwapBitmap();
}

void	FileStatusField::SetFileStatus(FileStatus status)
{

		
		if(status!=fStatus)
		{
		
		fWidth = 0;
		fClippedString.SetTo("");
		fStatus=status;
		SwapBitmap();
		
						
		switch(fStatus){
				case NO_ENCLOSURE:
					fOriginalStatus.SetTo(" ");		
				break;
				case NEW:
			 		fOriginalStatus.SetTo(B_TRANSLATE("new"));			 			
			 	break;
			 	case NOT_DOWNLOADED:
			 	  	fOriginalStatus.SetTo(B_TRANSLATE("not downloaded"));
			 	break;
		     	case DOWNLOADED:
			 	 	fOriginalStatus.SetTo(B_TRANSLATE("downloaded"));
			 	break;
		     	case ERROR:
			 	 	fOriginalStatus.SetTo(B_TRANSLATE("error"));
			 	break;
		     	case STOPPED:
		     	 	 	fOriginalStatus.SetTo(B_TRANSLATE("stopped"));
			 	break;
		     	case NOT_FOUND:
			 	 	fOriginalStatus.SetTo(B_TRANSLATE("not found"));
			 	break;
			 	case CANT_CONNECT:
			 	 	fOriginalStatus.SetTo(B_TRANSLATE("can't connect"));
			 	break;
		     	case DOWNLOADING:
			 	 	fOriginalStatus.SetTo(B_TRANSLATE("downloading"));
			 	break;
			 	case ENQUEQUED:
			 		fOriginalStatus.SetTo(B_TRANSLATE("enquequed"));
			 	break;
			 	case CONNECTING:
			 		fOriginalStatus.SetTo(B_TRANSLATE("connecting"));
			 	break;
			 	default:
			 		fOriginalStatus.SetTo(B_TRANSLATE("error"));
			 	break; 
		 }
		 	fString=fOriginalStatus;
		 	if(fStatus==STOPPED){
		 		//pervert game
		 		int perv=fPercentage;
		 		SetFilePercentage(0);
		 		SetFilePercentage(perv);
		 	}
		}
		
}

void				
FileStatusField::SetFilePercentage(int per,float speed)
{
	if(fPercentage==per) return;
	
	fWidth = 0;
	SwapBitmap();
	fClippedString.SetTo("");
	fPercentage=per;
	
	if(fStatus == STOPPED ||
	   fStatus == DOWNLOADING )
	   {
	   		BString sp;
	   		sp << per << "% " ;
	   	
	   		if(speed>0 &&  fStatus == DOWNLOADING )
	   		{
				
				float		size=speed;
				char		str[256];
				if (size < kKB_SIZE)
				{
					sprintf(str, "%.0f b/s", size);
				}
				else
				{
					const char*	suffix;
					float 		float_value;
					if (size >= kTB_SIZE)
					{
						suffix = "TB/s";
						float_value = (float)size / kTB_SIZE;
					}
					else if (size >= kGB_SIZE)
					{
						suffix = "GB/s";
						float_value = (float)size / kGB_SIZE;
					}
					else if (size >= kMB_SIZE)
					{
						suffix = "MB/s";
						float_value = (float)size / kMB_SIZE;
					}
					else
					{
						suffix = "k/s";
						float_value = (float)size / kKB_SIZE;
					}
					
					sprintf(str, "%.2f %s", float_value, suffix);
					// strip off an insignificant zero so we don't get readings
					// such as 1.00
					char *period = 0;
					char *tmp (NULL);
					for (tmp = str; *tmp; tmp++)
					{
						if (*tmp == '.')
							period = tmp;
					}
					if (period && period[1] && period[2] == '0')
						// move the rest of the string over the insignificant zero
						for (tmp = &period[2]; *tmp; tmp++)
							*tmp = tmp[1];
		   		
	   		
	   			sp << str << " "; //speed; 
	   		}
	   		sp << fOriginalStatus;
	   		fString = sp;
	   }	
	   }
}

void		
FileStatusField::SwapBitmap(){
	
	if(fBar==fBar1) fBar=fBar2;
		else fBar=fBar1;
	
}

//--------------------------------------------------------------------

void FileStatusField::SetString(const char* val)
{
	fString = val;
	fClippedString = "";
	fWidth = 0;
} 


//--------------------------------------------------------------------

const char* FileStatusField::String() const
{
	return fString.String();
}


//--------------------------------------------------------------------

void FileStatusField::SetWidth(float width)
{
	fWidth = width;
}

//--------------------------------------------------------------------

float FileStatusField::Width()
{
	return fWidth;
}


//--------------------------------------------------------------------

void FileStatusField::SetClippedString(const char* val)
{
	fClippedString = val;
} 


//--------------------------------------------------------------------

const char* FileStatusField::ClippedString()
{
	return fClippedString.String();
}


//=====================================================================

FileStatusColumn::FileStatusColumn(const char* title, float width, float minWidth,
							 float maxWidth, uint32 truncate, alignment align)
	:BTitledColumn(title, width, minWidth, maxWidth, align),
	fTruncate(truncate)
{
		if(fBar1  ==NULL) fBar1=LoadIcon("fullbar1.png");
		if(fBar2==NULL) fBar2=LoadIcon("fullbar2.png");
		if(fBack==NULL) fBack=LoadIcon("graybar.png");
}


//--------------------------------------------------------------------

void FileStatusColumn::DrawField(BField* _field, BRect rect, BView* parent)
{
	
	FileStatusField*	field = static_cast<FileStatusField*>(_field);
	
	float			width = rect.Width() - (2 * kTEXT_MARGIN);
	float 			basePoint = 0;
	
	if(field->GetFileStatus() == STOPPED ||
	   field->GetFileStatus() == DOWNLOADING )
	   {
			basePoint = 100 + kSPACE_TEXT;
	   }
	   
	if (width - basePoint != field->Width())
	{
		BString		out_string(field->String());

		parent->TruncateString(&out_string, fTruncate, width + 2 - basePoint);
		field->SetClippedString(out_string.String());
		field->SetWidth(width - basePoint);
	}
	
	if(basePoint>0){
	
		DrawBar(parent,rect,field->GetPercentage(),field->Bar());
	}	
	rect.left +=basePoint;
	DrawString(field->ClippedString(), parent, rect);
}


	
void
FileStatusColumn::DrawBar(BView* parent,BRect rect,int number,BBitmap* fBar)
{

	
	parent->PushState();
	parent->SetDrawingMode( B_OP_ALPHA );
	parent->SetBlendingMode( B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	
	BRect graphRect(rect);
	graphRect.right = graphRect.left+99;
	graphRect.top +=2;
	graphRect.bottom = graphRect.top + 11;
	
	parent->DrawBitmap(fBack,graphRect); //BPoint(rect.right,rect.top));
	

	BRect sourceRect(fBar->Bounds());
	sourceRect.right =  sourceRect.left + number;
		
	BRect destRect(graphRect);
	destRect.right = destRect.left + number; 
	parent->DrawBitmap(fBar,sourceRect,destRect); //sourceRect,destRect);
	
	parent->PopState();
	
}


//--------------------------------------------------------------------

int FileStatusColumn::CompareFields(BField* field1, BField* field2)
{
	return(ICompare(((FileStatusField*)field1)->String(),
				   (((FileStatusField*)field2)->String())));
}


//--------------------------------------------------------------------

bool FileStatusColumn::AcceptsField(const BField *field) const
{
	return static_cast<bool>(dynamic_cast<const FileStatusField*>(field));
}

