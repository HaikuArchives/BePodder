#include "SettingsTextView.h"
#include "Utils.h"

#define COLOR 255,255,255  //240,240,96

SettingsTextView::SettingsTextView(BRect rect, char *name,BString testo) 
	: BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW){
		
	fText=testo;
	SetViewColor(COLOR); 
}

void SettingsTextView::AttachedToWindow(){
		SetFontSize(12);
}


void SettingsTextView::Draw(BRect updateRect)
{
//		MovePenTo(BPoint(10,25));
		SetHighColor(0,0,0);
		SetLowColor(COLOR); 
		
		//Utils.cpp function:	
		BPDrawString(fText.String(),this,Bounds(),B_ALIGN_CENTER);
//		DrawString(fText.String());
}
