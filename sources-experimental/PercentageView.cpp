 #include "PercentageView.h"
 #include <Bitmap.h>
 #include "Utils.h"
 
 BBitmap*	empty = NULL ; 
 BBitmap*	bar1   = NULL ; 
 BBitmap*	bar2   = NULL ; 
 

PercentageView::PercentageView(BRect rect):BView(rect,"percentageView",B_FOLLOW_NONE,B_WILL_DRAW){

	if(!empty)
		empty = LoadIcon("graybar.png");	
	if(!bar1)
		bar1 = LoadIcon("fullbar1.png");
	if(!bar2)
		bar2 = LoadIcon("fullbar2.png");
	
	bar = bar1;
	
	fPercentage = 0;
}

void
PercentageView::AttachedToWindow(){
	if(Parent())
		SetViewColor(Parent()->ViewColor());
}
	
void
PercentageView::SetPercentage(int32 percentage){
	
	if(percentage<0) percentage=0;
	if(percentage>100) percentage=100;
	
	fPercentage=percentage;
	
	if(bar == bar1)
		bar = bar2;
	else
		bar = bar1;
		
	Invalidate();
}

void 
PercentageView::ResizeToPreferred(){

		ResizeTo(99,11);
		
}

void
PercentageView::Draw(BRect)
{
	
	SetDrawingMode( B_OP_ALPHA );
	
	BRect graphRect(Bounds());
	graphRect.right = graphRect.left+99;
	//graphRect.top +=2;
	graphRect.bottom = graphRect.top + 11;
	
	if(empty) DrawBitmap(empty,graphRect); //BPoint(rect.right,rect.top));
	
	if(fPercentage >0) {
	
		BRect sourceRect(bar->Bounds());
		sourceRect.right =  sourceRect.left + fPercentage;
		
		BRect destRect(graphRect);
		destRect.right = destRect.left + fPercentage; 
		if(bar) DrawBitmap(bar,sourceRect,destRect); //sourceRect,destRect);
	}
}	
