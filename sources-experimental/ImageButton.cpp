#include "ImageButton.h"

#include <string.h>
#include <stdio.h>


#include "Utils.h"
#include "IActionBP.h"
#include "IActionManagerBP.h"

ImageButton::ImageButton( BRect rect, const char * name, BMessage * msg,BBitmap * bitmap = NULL, BBitmap * bitmap_down = NULL , const char * label = NULL)
:	BControl(rect,name,label,msg,B_FOLLOW_NONE,B_WILL_DRAW)
{
	fBitmap = bitmap;
	eBitmap = bitmap_down;
	fMouseUp = false;
	fMouseDown = false;
	fMouseOver = false;
	fDrawBackground = true;
	fInset = BPoint(4,2);
	SetViewColor(B_TRANSPARENT_COLOR);
	if(label) SetToolTip(label);
}


ImageButton::ImageButton( BRect rect, const char*  name, IAction* action)
:	BControl(rect,name,action->GetLabel().String(),action->CopyMessage(),B_FOLLOW_NONE,B_WILL_DRAW)
{

	fBitmap = action->GetIcon(IActionBP::SIZE_48);
	eBitmap = action->GetIcon(IActionBP::SIZE_48_PRESSED); //bitmap_down;
	fMouseUp = false;
	fMouseDown = false;
	fMouseOver = false;
	fDrawBackground = true;
	fInset = BPoint(4,2);
	
	SetTarget(IActionManager::TargetForAction(0));
	SetToolTip(action->GetDescription().String());
	SetViewColor(B_TRANSPARENT_COLOR);

}
		

ImageButton::~ImageButton(){
}

void
ImageButton::MouseDown( BPoint /*where*/ )
{
	SetMouseEventMask( B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS );
	
	fMouseDown = true;
	
	Invalidate();
}

void
ImageButton::MouseMoved( BPoint /*where*/, uint32 transition, const BMessage * /*msg*/ )
{
	switch ( transition )
	{
		case B_ENTERED_VIEW:

			fMouseOver = true;
			Invalidate();
			break;
		case B_EXITED_VIEW:
	
			fMouseOver = false;
			Invalidate();
			break;
	}
	
}

void
ImageButton::MouseUp( BPoint where )
{
	if ( fMouseDown && Bounds().Contains(where) ){
		Invoke();
	}
	
	fMouseDown = false;
	
	Invalidate();
}

void	
ImageButton::SetBitmap(BBitmap* bitmap)
{
	fBitmap = bitmap;
}

void	
ImageButton::DrawbackgroundColor(bool draw)
{
	fDrawBackground = draw;
}
		
void
ImageButton::Draw( BRect )
{
	BRect plain(Bounds());
	plain.InsetBy(fInset.x,fInset.y);
	plain.bottom--;
	BBitmap*	draw=fBitmap;
	
	SetDrawingMode( B_OP_ALPHA ); //can be moved?
	
	if ( fMouseDown ) {
		SetHighColor( 242,227,0);
		draw=eBitmap;
	}
	else
		SetHighColor( 255,255,255);

	if (fDrawBackground)
		FillRect( plain );
	
	if (draw) 	
		DrawBitmap(draw, BPoint( 0,0 ));	
}
