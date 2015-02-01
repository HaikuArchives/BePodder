#ifndef HorizontalContainerBox_H_
#define HorizontalContainerBox_H_

#include "ContainerBox.h"


// here only the logic of the horiz stack model

#define MIN_COLUMN_WIDTH	30

class HorizontalContainerBox : public ContainerBox {

	public:
	
	HorizontalContainerBox(BRect preferredSize,BString txt,BoxRenderer* render,float par_space):
		ContainerBox(preferredSize,txt,render){
			fParSpace = par_space;
			left_margin = top_margin = 5.0;
		}
	
	void			
	InitSize(float maxwidth)  {
	
		rect.right = rect.left + maxwidth;
			
		fStackRect = BRect(0    + left_margin,
						   0    + top_margin,
						   0	+ maxwidth - left_margin - 1,
						   0    + top_margin); //0 height box!	   
		maxH = 0;
	};	
	
	void	
	UpdateSize(InstanceBox* box){
		fStackRect.left = box->rect.right + left_margin;
		if(box->rect.bottom > maxH) maxH = box->rect.bottom;
		SetSize(rect.Width(),maxH + left_margin + 1);
	}
	
	void	
	ArrangeBoxPosition(InstanceBox* box){
			box->rect = fStackRect;
			if(box->rect.Width() < MIN_COLUMN_WIDTH )
			{
				//si dovrebbe andare a capo..
				
			}	
	}	
		
		
				
			
	private:
		BRect	fStackRect;
		float	fParSpace;
		float 	maxH;
};
#endif
//--
