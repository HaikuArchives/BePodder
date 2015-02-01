#include <Window.h>

#include <TextView.h>

#define		DOWNLOAD_CHANNEL			'addu'

class PBox;

class AddWindow: public BWindow
{




 public:
					 AddWindow();
		void	MessageReceived(BMessage *mess);

	  			PBox*	text;
	  			BRect addrect;

 				
		
};

