/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "TitleTextRender.h"


TitleTextRender::TitleTextRender(){
	fFont = *be_bold_font;
	
};

TitleTextRender::TitleTextRender(BFont font){
	fFont = font;
	
};

TitleTextRender::~TitleTextRender(){
}
                        
void
TitleTextRender::Render(BView *target,const char* txt,int16 num,BPoint pos,BRect r){
	target->SetFont(&fFont);   
    target->DrawString(txt,num,pos); 
    target->StrokeLine(pos + BPoint(0,1) , pos + BPoint(fFont.StringWidth(txt),1),B_MIXED_COLORS);
}

void
TitleTextRender::GetHeight(font_height *height){
	fFont.GetHeight(height); 
    height->ascent += 5;
    height->descent +=5;
}

void
TitleTextRender::GetEscapements(const char charArray[], int32 numChars,float escapementArray[]){
	fFont.GetEscapements(charArray,numChars,escapementArray);
}

float
TitleTextRender::Size(float maxwidth){
	return fFont.Size();
}
