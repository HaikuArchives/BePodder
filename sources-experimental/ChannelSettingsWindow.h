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

#define SETTINGS_CHANNEL 	"channel_setting"
#define APPLY_CHANNEL_SETTINGS 'achs'

class ChannelSettingsWindow: public SettingsWindow
{




 public:
					 			ChannelSettingsWindow(BMessage*,BHandler*);
			
			
			
		
					
 private:
 
 					
 					
 					BBox*	CreateBox(BMessage);
 };
