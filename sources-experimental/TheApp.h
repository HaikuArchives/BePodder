#include <Application.h>

#include "MainWindow.h"
#include "MainController.h"

class TheApp : public BApplication {

public:
		TheApp(const char * signature);
		
		void MessageReceived(BMessage*);
		void ArgvReceived(int32 argc, char **argv) ;
		
		void		AboutRequested();
		void		ReadyToRun();
		bool		QuitRequested();

		void		ChekUpdate();

private:
	
		void	InstallMimeTypes();
	
	
		void	ApplySettings();
		void	ApplyWindowSettings();
	
		MainController*	fController;


};
