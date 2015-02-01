/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "GreenBoxTextRender.h"




GreenBoxTextRender::GreenBoxTextRender(){
	fFont = *be_fixed_font;
	fBackColor = greyBox;
};

GreenBoxTextRender::GreenBoxTextRender(BFont font,const rgb_color color){
	fFont = font;
	fBackColor = color;
};

GreenBoxTextRender::~GreenBoxTextRender(){
}
                        
void
GreenBoxTextRender::Render(BView *target,const char* txt,int16 num,BPoint pos,BRect rect){
	target->SetFont(&fFont);   
    target->DrawString(txt,num,pos); 

}

void
GreenBoxTextRender::GetHeight(font_height *height){
	fFont.GetHeight(height); 
    height->ascent += 2;
    height->descent +=2;
}

void
GreenBoxTextRender::GetEscapements(const char charArray[], int32 numChars,float escapementArray[]){
	fFont.GetEscapements(charArray,numChars,escapementArray);
}

float
GreenBoxTextRender::Size(float maxwidth){
	return fFont.Size();
}


 void	
 GreenBoxTextRender::MarginAfterTheText(BView *target,rgb_color view_color, BRect rect){
 	     	// Margin after fText
      							target->SetDrawingMode (B_OP_COPY);
      							//rect.right -=5;
      							target->SetLowColor (fBackColor);
      							target->FillRect (rect, B_SOLID_LOW);     		
 }
