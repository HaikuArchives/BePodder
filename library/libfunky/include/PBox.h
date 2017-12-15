#ifndef PBox_H
#define PBox_H

#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <Entry.h>
#include <GroupView.h>
#include <ListView.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <ObjectList.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <StringView.h>
#include <TextControl.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>
#include <String.h>


class BBox;
class BButton;

class PBox : public BGroupView  {
	/*
	enum {
		LISTCHANGED,
		SAVE,
		REVERT
	};
*/
	public:
		
								PBox(BRect rect,BMessage fTemplate, BMessage fData,const char* settings_name);

				 void			AttachedToWindow();
		virtual void			MessageReceived(BMessage *msg);

				void			GetData(BMessage* data);
				
				const char*		GetSettingsName() const {return fSName.String();};
	private:
		void					BuildGUI(BMessage viewTemplate, BMessage settings,BView *view);
		BMessage				fTemplate;
		BMessage				fData;
		BString					fSName;
};

#endif
