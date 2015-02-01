/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/


#ifndef  _GreenBoxTextRender_H_
#define _GreenBoxTextRender_H_

#include <View.h>
#include <Font.h>
#include "TextRender.h"

const rgb_color greyBox = {255,234,130,255};     //200,203,235    255,234,130  185,185,185     233,233,223

class GreenBoxTextRender : public TextRender
{
    public:
                        GreenBoxTextRender();
                        GreenBoxTextRender(BFont font,const rgb_color color);
                        virtual ~GreenBoxTextRender();
                        
       	 void   Render(BView *target,const char*,int16 num,BPoint pos,BRect rect);
       	 void 	GetHeight(font_height *height);
       	 void	GetEscapements(const char charArray[], int32 numChars,float escapementArray[]);
       	 float	Size(float maxwidth);
       //
       	 void	MarginAfterTheText(BView *target,rgb_color view_color, BRect rect);
       	 
            
    private:
    		BFont 			fFont;
    		rgb_color		fBackColor;
};
#endif
