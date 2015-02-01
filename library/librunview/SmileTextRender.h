#ifndef _SmileTextRender_H_
#define _SmileTextRender_H_

#include "TextRender.h"
#include <Font.h>
#include <View.h>

#include <stdio.h>
#include <TranslationUtils.h>
#include <Resources.h>
#include <String.h>

#include "Emoticor.h"

extern float gEmoticonSize;

class SmileTextRender : public TextRender
{
    public:
          SmileTextRender():TextRender(){};
          
          virtual ~SmileTextRender() {};        
       
       virtual void Render(BView *target,const char* txt,int16 num,BPoint pos)  {
           
            BBitmap *pointer=NULL;
            BString f(txt,num);
            
          	if(emoticor->config->FindPointer(f.String(),(void**)&pointer)==B_OK)
          	{
          		target->SetDrawingMode( B_OP_ALPHA );
          	 	target->DrawBitmapAsync( pointer,BPoint(pos.x, pos.y- (gEmoticonSize/2)) );
          		target->SetDrawingMode( B_OP_OVER );
          		
/*		       	BRect r = pointer->Bounds();
          		r.OffsetTo(pos);
          		r.OffsetBy(0, - (gEmoticonSize/2 + 2) );
          		r.InsetBy(-1,-1);
          		
          		rgb_color oldCol = target->HighColor();
          		target->SetHighColor( 255,0,0 );
          		target->StrokeRect( r );
          		target->SetHighColor( oldCol );
*/
          	}
        }; 
       
       
       virtual float Size(){ return gEmoticonSize;}
       
       virtual void GetHeight(font_height *h)
       { 
       		h->ascent=gEmoticonSize/2;
       		h->descent=gEmoticonSize/2;
       		h->leading=0;
       	};
    	
	   virtual void		
	   GetEscapements(const char * /*charArray*/, int32 numChars,float escapementArray[])
	   {
  			//font.GetEscapements(charArray,numChars,escapementArray);
  			escapementArray[0]=1;
  			for(int i=1;i<numChars;i++) escapementArray[i]=0;
  	   }
};
#endif
