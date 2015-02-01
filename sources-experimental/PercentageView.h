#ifndef 	PercentageView_H_
#define	PercentageView_H_

#include <View.h>

class BBitmap;

class PercentageView : public BView {

	public:
				PercentageView(BRect);
				
				void	SetPercentage(int32);

				int32 GetPercentage();
				
				void	Draw(BRect rect);
				
				void	AttachedToWindow();
				void ResizeToPreferred();
				
	private:
				int32	fPercentage;
				 BBitmap*	bar ; 
					
};

#endif
//--
