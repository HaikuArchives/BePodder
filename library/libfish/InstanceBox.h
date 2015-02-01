#ifndef InstanceBox_H_
#define InstanceBox_H_

class BoxRenderer;

#include "String.h"

class InstanceBox {

public:
		
		InstanceBox(BRect preferredSize,BString txt,BoxRenderer* render){
			box_alignment = B_ALIGN_LEFT;
			text=txt;
			renderer=render;
			rect = preferredSize;
			hlimit = -1;
		}
		
		void SetSize(float w,float h){
			rect.SetRightBottom(rect.LeftTop() + BPoint(w-1,h-1));
		}
		
		BString			text;
		BRect			rect;
		BoxRenderer*	renderer; //don't like this.
		
		float			hlimit;
		
		alignment  		box_alignment; 
		//this is used by the renderer..:
		void*			cookie;
		BString			action; 
};
#endif
//--
