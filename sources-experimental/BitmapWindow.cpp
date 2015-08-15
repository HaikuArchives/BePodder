#include "BitmapWindow.h" 

#include "Utils.h"
#include <View.h>
#include "BPLocale.h"
#include "BitmapView.h"

BitmapWindow::BitmapWindow(BBitmap* image) :
	BWindow(BRect(162,50,848,700),_T("Channel image"), B_TITLED_WINDOW,B_NOT_RESIZABLE| B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS|B_WILL_DRAW)
{
	ResizeTo(image->Bounds().Width(),image->Bounds().Height());
	AddChild(new BitmapView(image->Bounds(),image));
}


//bool
//BitmapWindow::QuitRequested(){
//	return true;
//}





