#ifndef VerticalContainerBox_H_
#define VerticalContainerBox_H_

#include "ContainerBox.h"


// here only the logic of the vertical stack model


class VerticalContainerBox : public ContainerBox {

	public:
	
	VerticalContainerBox(BRect preferredSize,BString txt,BoxRenderer* render,float par_space):
		ContainerBox(preferredSize,txt,render){
			fParSpace = par_space;
			//fMargin = margin;	
			left_margin = top_margin = 5.0;
		}
	
	
	void			
	InitSize(float maxwidth)  {
		
		rect.right = rect.left + maxwidth;
			 
		fStackRect = BRect(0    + left_margin,
						   0    + top_margin,
						   0	+ maxwidth - left_margin - 1,
						   0    + top_margin); //0 height box!	
		//fStackRect.PrintToStream();				   
		
	};	
	
	void	
	UpdateSize(InstanceBox* box){
			fStackRect.OffsetTo(fStackRect.left,box->rect.bottom);
			SetSize(rect.Width(),fStackRect.bottom + left_margin );
			//rect.PrintToStream();				
			fStackRect.top = fStackRect.bottom += fParSpace;
	}
	
	void	
	ArrangeBoxPosition(InstanceBox* box){
			box->rect.top    = fStackRect.bottom;
			box->rect.left   = fStackRect.left;
			box->rect.bottom = box->rect.top; // 0
			box->rect.right  = fStackRect.left + fStackRect.right - left_margin; // max size
			//box->rect.PrintToStream();
	}	
		
	float			MaxWidth(){ return fStackRect.Width();}
	
	private:
		BRect fStackRect;
		float	fParSpace;
		//float	fMargin;

};
#endif
//--
