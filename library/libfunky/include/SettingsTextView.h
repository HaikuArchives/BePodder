#ifndef SettingsTextView_H_
#define SettingsTextView_H_

#include <Window.h>
#include <View.h>
#include <String.h>


class SettingsTextView : public BView 
{
		public:
						SettingsTextView(BRect frame, char *name, BString testo);
			 void AttachedToWindow();
			 void Draw(BRect updateRect);
			 void	SetText(const char*text){fText.SetTo(text);};
			 BString		Text(){ return fText; }
		private:
				BString fText;
		
		
		

};

#endif
