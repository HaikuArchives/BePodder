#ifndef   HelpWindow_H
#define  HelpWindow_H
 
#include <Window.h>

class HelpViewer;
class BButton;

class HelpWindow: public BWindow
{



 public:
					HelpWindow();
		 void 		MessageReceived(BMessage* message);
		 void		SetPage(BString id);


 private:
 		
 		int Selected;
 		void	SetPage(int i);		
 		HelpViewer* hv;
		BButton *back;
		BButton *next;	
};

#endif

//sas
