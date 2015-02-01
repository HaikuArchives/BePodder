#include <Control.h>

#define MAX_ITEM	5

class SectionSelector : public BControl
{
	public:
				 	SectionSelector(BRect,BMessage* msg);
			
			void 	Draw(BRect);
		//	void 	MouseUp(BPoint);
					
			void	AddSection(const char* iconName,BView* tabView,const char* tooltip);
			
			void	Select(int index);
			
	   		
			int					Selected(){ return fSelected; };
	
	protected:
	
			const char*		NameOf(int index);
	
	private:
			
			struct 	Section {
				
				//BRect	 rect;
				//BBitmap* icon;
				BView*	 view;
				BView*	 tab;					
			};
			
			void	DrawString(const char* string, BView* parent, BRect rect);
			void	DrawPart(int index);
			
	//		BRect	RectFor(int i);
			
			int 	fSelected;
			float	width;
			int		count;
			
			Section*		fStack[MAX_ITEM];
			
};

//--
