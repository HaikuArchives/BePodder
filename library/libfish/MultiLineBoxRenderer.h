#ifndef MultiLineBoxRenderer_H_
#define MultiLineBoxRenderer_H_

#include "BoxRenderer.h"
#include <List.h>
#define kTEXT_MARGIN	0

struct multi_line {
	int		count;
	BList	line;
};

class MultiLineBoxRenderer : public BoxRenderer {

	public:
					MultiLineBoxRenderer(BFont fnt){
						font = fnt;
						align = B_ALIGN_RIGHT;
					}
	
					
	void	DrawBox(BView* parent,InstanceBox* ibox) {
		parent->StrokeRect(ibox->rect);
		parent->SetFont(&font);		
		multi_line *ml=(multi_line*)ibox->cookie;
		BRect rect(ibox->rect);
		rect.bottom = rect.top + font.Size() - 1;
		for(int i=0;i<ml->count;i++){
			BString*	str=(BString*)ml->line.ItemAt(i);
			DrawString(str->String(),parent,rect);
			rect.OffsetBy(0, font.Size());
		}
		
	};
	
	float	GetHeight(InstanceBox* ibox ) {
		multi_line *ml=(multi_line*)ibox->cookie;
		return font.Size()*ml->count;
	};
	
	float	GetWidth(InstanceBox* ibox ) {

		multi_line *ml=(multi_line*)ibox->cookie;
		
		float maxsize=0;
		for(int i=0;i<ml->count;i++){
			BString*	str=(BString*)ml->line.ItemAt(i);
			float size = font.StringWidth(str->String());
			if(size > maxsize) maxsize=size;
		}
		return maxsize;
	};


	void	
	InitInstanceBox(InstanceBox* ibox,HDoc*)
	{
		//debugger("instance");
		//devo dividere il testo in più linee e farne un array di stringhe.
		multi_line *ml=new multi_line;
		ml->count=0;
		
		int32 index = 0;
		int32 past_index = 0;
		
		index = ibox->text.FindFirst("\n",past_index);
		
		while( index > 0){
			ml->count++;
			BString *s=new BString();
			ibox->text.CopyInto(*s,past_index,index-past_index);
			ml->line.AddItem(s);
			
			past_index = index + 1;
			index = ibox->text.FindFirst("\n",past_index);
		}
		
		index = ibox->text.FindLast("\n",past_index);
		if(index > 0){
			ml->count++;
			BString *s=new BString();
			ibox->text.CopyInto(*s,past_index,index-past_index);
			ml->line.AddItem(s);
		}
		else
		{
			ml->count=1;
			ml->line.AddItem(&ibox->text);
		}
		
		ibox->cookie=(void*)ml;
		
			ibox->SetSize(this->GetWidth(ibox),this->GetHeight(ibox));
			
	}
	


	void DrawString(const char* string, BView* parent, BRect rect)
{
	float		width = rect.Width() - (2 * kTEXT_MARGIN);
	float		y;
	BFont		font;
	font_height	finfo;

	parent->GetFont(&font);
	font.GetHeight(&finfo);
	y = rect.top + ((rect.Height() - (finfo.ascent + finfo.descent + finfo.leading)) / 2) +
					(finfo.ascent + finfo.descent) - 2;

	switch (Alignment())
	{
		case B_ALIGN_LEFT:
			parent->MovePenTo(rect.left + kTEXT_MARGIN, y);
			break;

		case B_ALIGN_CENTER:
			parent->MovePenTo(rect.left + kTEXT_MARGIN + ((width - font.StringWidth(string)) / 2), y);
			break;

		case B_ALIGN_RIGHT:
			parent->MovePenTo(rect.right - kTEXT_MARGIN - font.StringWidth(string), y);
			break;
	}
	parent->DrawString(string);
}

	void SetAlignment(alignment flag){
			align=flag;		
	};
	
	alignment Alignment(void) const {
			return align;				
	};
	

	private:
				BFont	font;
				alignment align;
				
};
#endif
//--
