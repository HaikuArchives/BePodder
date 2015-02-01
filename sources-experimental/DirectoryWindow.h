#include <Window.h>
#include <Box.h>
#include <CheckBox.h>
#include <RadioButton.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <TextControl.h>
#include <PopUpMenu.h>
#include <OutlineListView.h>

#include "OPMLParser.h"

#include "BPAlert.h"

#include "KeyMap.h"

class	PercentageWindow;

class DirectoryWindow: public BWindow
{




 public:
					 DirectoryWindow(const char* opml_name,BString opml_url); //,const rgb_color back_color);
					// ~DirectoryWindow();
					 
		void	MessageReceived(BMessage *mess2);
		
	  	
					

private:

		PercentageWindow*	fAlert;

		void	StartDownload(BString url);


		void	DownloadInfo(BMessage*);


		//void	SetDescription(const char*);
		void SetError(const char* tex);
		void	ApplyTree(OPMLTree*);
		
		void	deletePath(BMessage* msg);
		
		void	_addItem(BListItem* root,OPMLTree*);
		BOutlineListView *alist; 	
				
		BString	fOpmlName;	
		BString	fOpmlURL;
		
	
		
};

