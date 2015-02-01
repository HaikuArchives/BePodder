/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/


#ifndef _TitleTextRender_H_
#define _TitleTextRender_H_

#include <View.h>
#include <Font.h>
#include "TextRender.h"

class TitleTextRender : public TextRender
{
    public:
                        TitleTextRender();
                        TitleTextRender(BFont font);
                        virtual ~TitleTextRender();
                        
       	 void   Render(BView *target,const char*,int16 num,BPoint pos,BRect r);
       	 void 	GetHeight(font_height *height);
       	 void	GetEscapements(const char charArray[], int32 numChars,float escapementArray[]);
       	 float	Size(float maxwidth);
       	 
       //
            
    private:
    		BFont fFont;
};
#endif
