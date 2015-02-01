#include <Window.h>
#include <View.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <Alert.h>
#include <RadioButton.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <TextControl.h>
#include <PopUpMenu.h>
#include <Message.h>
#include <ListView.h>
#include <ObjectList.h>

#include "SettingsWindow.h"

#define SETTINGS_MAINWINDOW 	"main_window_setting"
#define SETTINGS_FILETYPE		"filetype_setting"
#define SETTINGS_PROXY			"proxy_setting"
#define SETTINGS_DOWNLOAD		"download_setting"
#define SETTINGS_CHANNELS		"channels_setting"
#define SETTINGS_GENERAL		"general_setting"
#define SETTINGS_UPDATES		"updates_setting"

#define SETTINGS_GROUPS		"groups_setting"


class BPSettingsWindow: public SettingsWindow
{




 public:
					 			BPSettingsWindow();
			
			
			
		
					
 private:
 
 					
 					
 					BBox*	CreateGeneralBox(BMessage);
 					BBox*	CreateFileTypeBox(BMessage);
 					BBox*	CreateChannelsBox(BMessage);
 					BBox*	CreateProxyBox(BMessage);
 					BBox*	CreateDownloadBox(BMessage);
 					
 					BBox*	CreateUpdateBox(BMessage);
 					
 					
 					
 					void	FillOfValue(BView*); //this name is fake! ;)
 					void	FixLabel(BView*); //this name is fake! :p
 };
