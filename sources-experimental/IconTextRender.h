#ifndef _IconTextRender_H_
#define _IconTextRender_H_

#include "TextRender.h"
#include "Bitmap.h"
#include <Font.h>
#include <View.h>

#include <stdio.h>
#include <TranslationUtils.h>
#include <Resources.h>
#include <String.h>

#define	kIconSpace	3.0

class IconTextRender : public TextRender
{
    public:
       
       IconTextRender():TextRender(){
        fHeight = 0; // icon->Bounds().Height() + 1;
		fWidth = 0;       
	};
       
       virtual ~IconTextRender() {};        
       
       void Render(BView *target,const char* txt,int16 num,BPoint /*pos*/,BRect r)  {
           
            BBitmap *pointer =NULL;
            BString key(txt,num);
            
            if( fKeyBit.FindPointer(key.String(),(void**)&pointer) !=B_OK ) {
              	return;
            }
            if(!pointer) return;
            
           	target->SetDrawingMode( B_OP_ALPHA );
           	target->DrawBitmapAsync( pointer,BPoint(r.left + kIconSpace, r.top ));
		//target->DrawBitmapAsync( pointer,BPoint(r.left + kIconSpace, r.top + ((r.Height()/2.0) - 8)  ));       //- fHeight + (fHeight/4) + 2)
          	target->SetDrawingMode( B_OP_OVER );
         }; 
       
       
       void		
       AddIcon(const char* key,BBitmap* icon){
       		fKeyBit.AddPointer(key,icon);
       }
       
       float Size(float/* maxwidth*/){ return fWidth + 1 + kIconSpace + kIconSpace;}
       
       void GetHeight(font_height *h){ 
       		h->ascent=fHeight/2;
       		h->descent=fHeight/2;
       		h->leading=0;
       	};
    	
	   void		
	   GetEscapements(const char * /*charArray*/, int32 numChars,float escapementArray[]) {
  			escapementArray[0]=1;
  			for(int i=1;i<numChars;i++) escapementArray[i]=0;
  	   }
  	   
	  void SetIconSize(float h,float w){
	  	fHeight = h + 1;
		fWidth = w;
	  }  	   

  	   private:
  	  	 	float fHeight;
			float fWidth;
  	   		BMessage	fKeyBit;
};
#endif
