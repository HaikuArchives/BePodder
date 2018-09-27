#ifndef _TextRender_H_
#define _TextRender_H_

class TextRender
{
    public:
                        TextRender(){};
                        virtual ~TextRender() {};
                        
       virtual void     Render(BView *target,const char*,int16 num,BPoint pos,BRect drawRect) = 0;
       
       virtual void 	GetHeight(font_height *height) = 0;
       virtual void		GetEscapements(const char charArray[], int32 numChars,float escapementArray[])=0;
       virtual float	Size(float maxwidth) = 0;
       
       virtual void		MarginAfterTheText(BView *target,rgb_color view_color, BRect rect){
       					      	// Margin after fText
      							target->SetDrawingMode (B_OP_COPY);
      							target->SetLowColor (view_color);
      							target->FillRect (rect, B_SOLID_LOW);     								
       					}
       //
            
};
#endif
