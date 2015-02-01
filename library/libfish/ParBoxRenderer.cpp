#include "ParBoxRenderer.h"
#include "TextInstanceBox.h"


#include <stdio.h>
#include <malloc.h>
	
#define kTEXT_MARGIN	0



inline int32
UTF8_CHAR_LEN (uchar c)
{
 return (((0xE5000000 >> (((c) >> 3) & 0x1E)) & 3) + 1);
}


void addLine(const char* line,int size,multi_line* ml){
	ml->count++;
	BString *s=new BString(line,size);
	//ibox->text.CopyInto(*s,line,line + size);
	//printf("Added line %d [%s]\n",ml->count-1,s->String());
	ml->line.AddItem(s);

}

InstanceBox*	
ParBoxRenderer::CreateInstanceBox(BRect preferredSize,BString txt){
	return new TextInstanceBox(preferredSize,txt,this);
}

void	
ParBoxRenderer::DrawBox(BView* parent,InstanceBox* ibox) {
		TextInstanceBox* t = (TextInstanceBox*)ibox;
		parent->SetFont(&t->text_font);		
		multi_line *ml=(multi_line*)ibox->cookie;
		if(!ml) return;
		BRect rect(ibox->rect);
		rect.bottom = rect.top + t->font_full_height -1; //t->text_font.Size() - 1;
		for(int i=0;i<ml->count;i++){
			BString*	str=(BString*)ml->line.ItemAt(i);
			DrawString(str->String(),parent,rect,B_ALIGN_LEFT);
			rect.OffsetBy(0, t->font_full_height);
		}
};

void	
ParBoxRenderer::InitInstanceBox(InstanceBox* ibox,ContainerBox*) {
	// devo dividere il testo in più linee e farne un array di stringhe.
	// in base agli spazi e non hai \n.
	
	TextInstanceBox* t = (TextInstanceBox*)ibox;
	
	multi_line *ml=new multi_line;
	ml->count=0;

	
	//first: remove all the \n
	ibox->text.ReplaceAll("\n"," ");
	ibox->text.RemoveAll("\t");
	
	float maxwidth=ibox->rect.Width();
	
	if(t->hlimit>0 && t->hlimit<maxwidth) maxwidth=t->hlimit;
	
	float width=0;
	const int16          fLength=ibox->text.Length();
	int16          *fSpaces;
	int16          fSpace_count;
	
	const char spacers[] = " \t\n-\\/";
  	const char *buffer(ibox->text.String());
  	size_t offset (0), n;
    int16 count (0);
	 
  	fSpace_count = 0;
  	while ((n = strcspn (buffer + offset, spacers)) < fLength - offset){
       ++count;
       offset += n + 1;
     }

  	 fSpaces = new int16 [count + 1];

  	 offset = 0;
  	 while ((n = strcspn (buffer + offset, spacers)) < fLength - offset){
     	fSpaces[fSpace_count++] = n + offset;
    	offset += n + 1;
  	 }
  	 
  	 fSpaces[fSpace_count++] = ibox->text.Length();
	
	 //printf("Text: %s\n",ibox->text.String());	
	 //for(int i=0;i<	fSpace_count;i++)
	 //	printf("Space at %d (position %d)\n",i,fSpaces[i]);
	
	if(fSpace_count<=0)
		debugger("no space?"); //impossible! (We just Append(" ")..)
	
		int curSpace = 0;
		int curChar  = 0;
		
		while(curSpace<fSpace_count){
			
			float pixel = 0;
			int from  = curChar;
			
			while(curSpace<fSpace_count ){
			
			 
			  
			 float size = StringSize(&t->text_font, 
			 						 buffer + curChar,
			 						 fSpaces[curSpace] - curChar);
			 {
				char copy[fSpaces[curSpace] - curChar + 1];
				memset(copy,0,fSpaces[curSpace] - curChar + 1);
				strncpy(copy,buffer + curChar,fSpaces[curSpace] - curChar);
				//printf("pixel (%s) until %d: %f   (curSpace %d)\n",copy,fSpaces[curSpace],size,curSpace);
			
			 }
			 
			 pixel += size;
			
			 if( pixel >= maxwidth && pixel > size) {
			  	addLine(buffer + from,(int)(curChar-from),ml);
			  	if(pixel>width) width=pixel;
			 	curChar++; //skip the space!
			   	break;
			 }
			 
			 else
			 if(pixel >= maxwidth && pixel == size){
			 	
			  	// a big word. brute split!
			  	int i;
			  	float size;
			 	
			 	for(i = fSpaces[curSpace] ; i > from; i-- ){  //UTF16 kind?
			 	 size = StringSize(&t->text_font,buffer + from, i - from);
			 	 if(size <= maxwidth){
			 	 	  //found the breakpoint!
			 	 	  addLine(buffer + from,i - from,ml);
			 	 	  if(pixel>width) width=pixel;
				      break;
				 }
				 
				}//for
				 if(i <= curSpace){
				 	 i++;
				 	 ibox->cookie = NULL;
				 	 delete fSpaces;
				 	 return;
				 	 //debugger("less then one char bug");
				 }
				 curChar =  i;
				 break;
			
			 }
			 
			 //qui controllo del flush ultra finale!
			 if(curSpace == fSpace_count-1) {
			  	addLine(buffer + from,fSpaces[curSpace] - from,ml);
			  	if(pixel>width) width=pixel;
			 }
			 
			 curChar = fSpaces[curSpace];
			 curSpace++;
			 
						
			}
			
			//printf("-----\n");
		}
		
	ibox->cookie=(void*)ml;
	font_height fh;
	t->text_font.GetHeight(&fh);
	t->font_full_height =  ceil (fh.ascent + fh.descent + fh.leading);
	if(width>ibox->rect.Width()) width = ibox->rect.Width();
	ibox->SetSize(width,t->font_full_height*ml->count);
	
	HAlignBox(ibox,maxwidth);
	
	delete fSpaces;
	
	return;

}

void
ParBoxRenderer::FreeInstanceBox(InstanceBox* box){
	if(box->cookie)
		delete ((multi_line*)box->cookie);
	
}
	
float	
ParBoxRenderer::StringSize(BFont* f,const char* string){
	 return f->StringWidth(string);
}

float	
ParBoxRenderer::StringSize(BFont* f,const char* string,int lenght){
	 return f->StringWidth(string,lenght);
}
	
	
void 
ParBoxRenderer::DrawString(const char* string, BView* parent, BRect rect,alignment ali)
{
	float		width = rect.Width() - (2 * kTEXT_MARGIN);
	float		y;
	BFont		font;
	font_height	finfo;

	parent->GetFont(&font);
	font.GetHeight(&finfo);
	y = rect.top + ((rect.Height() - (finfo.ascent + finfo.descent + finfo.leading)) / 2) +
					(finfo.ascent + finfo.descent) - 2;

	switch (ali)
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

