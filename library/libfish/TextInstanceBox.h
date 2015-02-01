#ifndef TextInstanceBox_H_
#define TextInstanceBox_H_

class BoxRenderer;
#include "Font.h"

class TextInstanceBox : public InstanceBox {

public:
		
		TextInstanceBox(BRect preferredSize,BString txt,BoxRenderer* render):
			InstanceBox(preferredSize,txt,render){
				text_font = *be_plain_font;
				font_height h;
				font_full_height = h.ascent + h.descent + h.leading;
			}
	
		BFont 		text_font;
		float		font_full_height;
		
};
#endif
//--
