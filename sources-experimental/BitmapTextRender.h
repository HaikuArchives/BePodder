#ifndef _BitmapTextRender_H_
#define _BitmapTextRender_H_

#include "TextRender.h"
#include "Bitmap.h"
#include <Font.h>
#include <View.h>

#include <stdio.h>
#include <TranslationUtils.h>
#include <Resources.h>
#include <String.h>


#define	kIconSpace	3.0

/* 
	ATTENZIONE
	
	This class owns the Bitmap pointer!
	It deletes the pointer as soon as you change it or you set it to NULL.

*/

class BitmapTextRender : public TextRender
{
    public:
       
       BitmapTextRender():TextRender(){
        fHeight = 0; 
        fWidth = 0;
        fBitmap=NULL;       
	};
       
       void LoadBitmap(const char* filename){
       		if(fBitmap!=NULL) delete fBitmap;
       		
       		
       		if(filename!=NULL)
       			fBitmap =  BTranslationUtils::GetBitmap(filename);
       		else 
       		    fBitmap = NULL;
       		
       		if(fBitmap){
       			  fHeight = fBitmap->Bounds().Height(); 
        		  fWidth = fBitmap->Bounds().Width();
       		}
       		else {
       			fHeight = 0;
       			fWidth = 0;
       		}
       		
       }
       
       virtual ~BitmapTextRender() {};        
       
       void Render(BView *target,const char* txt,int16 num,BPoint pos,BRect r)  {
           
            if(!fBitmap) return;
            
           	target->SetDrawingMode( B_OP_ALPHA );
           	target->DrawBitmapAsync( fBitmap,BPoint(r.left + kIconSpace, r.top ));
		//target->DrawBitmapAsync( pointer,BPoint(r.left + kIconSpace, r.top + ((r.Height()/2.0) - 8)  ));       //- fHeight + (fHeight/4) + 2)
          	target->StrokeRect(r);
          	target->SetDrawingMode( B_OP_OVER );
         }; 
       
       
      
       
       float Size(float maxwidth){ return fWidth + 1 + kIconSpace + kIconSpace;}
       
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
  	   
	 
  	   private:
  	  	 	float 			fHeight;
			float 			fWidth;
			BBitmap*	fBitmap;
			
};
#endif
