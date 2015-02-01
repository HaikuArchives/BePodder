#include <Window.h>
#include <TextView.h>
//#include "HelpView.h"


class HelpTextView : public BView 
{
		public:
		HelpTextView(BRect frame, char *name, BString testo, BPoint posizione, int grandezza_font, BFont tipo);
		virtual void AttachedToWindow();
		virtual void Draw(BRect updateRect);
		BString fText;
		BPoint point;
		BFont font;
		int size;
		
		
	
	
	
		
		
		

};

HelpTextView::HelpTextView(BRect rect, char *name, BString testo, BPoint posizione,int grandezza_font, BFont tipo) 
	: BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
fText=testo;
point=posizione;
size=grandezza_font;
font=tipo;


         SetViewColor(159,181,202); 

}

void HelpTextView::AttachedToWindow()
{
		
		SetFont(&font, B_FONT_FAMILY_AND_STYLE);
		SetFontSize(size);
}


void HelpTextView::Draw(BRect updateRect)
{
		MovePenTo(BPoint(point));
		SetHighColor(0,0,0);
		SetLowColor(159,181,202);

		
		DrawString(fText.String());
		
}

