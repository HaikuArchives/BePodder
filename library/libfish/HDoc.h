#ifndef HDoc_H_
#define HDoc_H_

#include "BoxRenderer.h"
#include "VerticalContainerBox.h"

class HDoc : public VerticalContainerBox { //ContainerBox {

	public:// 
	
		//refactoring: using a BRect. if rect.top==rect.bottom then canGrow = false!
		
		HDoc(BString fid,float w,float h,float margin,float parSpace):VerticalContainerBox(BRect(0,0,w-1,h-1),fid,NULL,parSpace){
			rgb_color white = {255,255,255,255};
			rgb_color black = {0,0,0,255};
			backColor = white;
			foreColor = black;
			textFontSize  = 14;
			titleFontSize = 20;
			left_margin = top_margin = 5.0;
		};

		BString	DocId(){ return text;}
	
	public: //TEMP fix me!
			rgb_color backColor;
			rgb_color foreColor;
			int		textFontSize;
			int		titleFontSize;
};
#endif
