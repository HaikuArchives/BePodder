#ifndef 	ImageButton_H
#define ImageButton_H

#include <Control.h>
#include <Bitmap.h>
#include <Point.h>

class IAction;

class ImageButton : public BControl
{
	public:
		
		ImageButton( BRect rect, const char * name, BMessage * msg, BBitmap * bitmap, BBitmap * bitmap_down, const char * label );
		ImageButton( BRect rect, const char*  name, IAction* action);
		virtual ~ImageButton();
		
		virtual void MouseDown( BPoint );
		virtual void MouseMoved( BPoint, uint32, const BMessage * );
		virtual void MouseUp( BPoint );
		
		virtual void Draw( BRect );
		
		void	SetInsetPoint(BPoint p) { fInset = p;}
		
		void	SetBitmap(BBitmap*);
		void	DrawbackgroundColor(bool);
	
	private:
		bool		fMouseDown, fMouseOver, fMouseUp;
		BBitmap		* fBitmap;
		BBitmap		* eBitmap;
		BPoint		fInset;
		bool		fDrawBackground;
};

#endif
