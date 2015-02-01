#include "TitleBoxRenderer.h"
#include "TextInstanceBox.h"

	
#define kTEXT_MARGIN	0

void
TitleBoxRenderer::DrawBox(BView* parent,InstanceBox* ibox) {
		TextInstanceBox* t = (TextInstanceBox*)ibox;
		parent->SetFont(&t->text_font);		
		multi_line *ml=(multi_line*)ibox->cookie;
		if(!ml) return;
		BRect rect(ibox->rect);
		rect.bottom = rect.top + t->font_full_height -1; //rect.top + t->text_font.Size() - 1;
		for(int i=0;i<ml->count;i++){
			BString*	str=(BString*)ml->line.ItemAt(i);
			BPoint pos(rect.left,rect.bottom + 1);
			parent->StrokeLine(pos , pos + BPoint(t->text_font.StringWidth(str->String()),0),B_MIXED_COLORS);
			DrawString(str->String(),parent,rect,B_ALIGN_LEFT);
			rect.OffsetBy(0, t->font_full_height);
			
		}
}
