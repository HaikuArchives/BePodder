#ifndef _NormalTextRender_H_
#define _NormalTextRender_H_

#include "TextRender.h"
#include <Font.h>
#include <View.h>

#include <stdio.h>

class NormalTextRender : public TextRender
{
    public:
    	  
    	 NormalTextRender(BFont f):TextRender(){
                        	font=f;
          }
          
         ~NormalTextRender() {};
          
       	void     Render(BView *target,const char* txt,int16 num,BPoint pos,BRect drawRect)  {
           
           target->SetFont(&font);   
           target->DrawString(txt,num,pos); 
          
           
       }; 
       
       
       float Size(float maxwidth){ return font.Size();}
       
       void GetHeight(font_height *height){ font.GetHeight(height); };
    
    
	   void GetEscapements(const char charArray[], int32 numChars,float escapementArray[]){
  			font.GetEscapements(charArray,numChars,escapementArray);
	   }
    
    private:
            BFont       font;
            
};
#endif
