#ifndef		_PercentageWindow_H_
#define	_PercentageWindow_H_

#include <Window.h>
#include <Looper.h>
#include <Bitmap.h>
#include <String.h>

class BStringView;
 class PercentageView;
 
class PercentageWindow : public BWindow {

	public:
							PercentageWindow(const char* title,const char* text,BBitmap* icon=NULL);
				void		Go(BLooper* lop=NULL,int32 msg=0);
				void		SetPercentage(int perc);
				bool		QuitRequested();
	
	private:
					BLooper*		fLooper;
					int32			fMsg;
					int kTextIconOffset;
					BStringView*		perc;
					PercentageView*	pw;

};

#endif
