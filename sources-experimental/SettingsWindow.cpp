#include "SettingsWindow.h"
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <ListView.h> 
#include <Message.h>
#include <StorageKit.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <StringView.h>
#include "PBox.h"
#include <Alert.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SettingsWindow"

SettingsWindow::SettingsWindow(BMessage* setting,BHandler* handler,int32 applyMsg) :
	BWindow(BRect(250,100,350,350),B_TRANSLATE("Preferences"), B_FLOATING_WINDOW_LOOK,B_MODAL_APP_WINDOW_FEEL,
	B_NOT_RESIZABLE| B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS|B_AUTO_UPDATE_SIZE_LIMITS)
{
	fApplyWhatMsg = applyMsg;
	fSetting = setting;
	fHandler = handler;
	
	fSettingsTypeListView = new BListView("preferences_list",
		B_SINGLE_SELECTION_LIST);
	fSettingsTypeListView->SetSelectionMessage(new BMessage('list'));

	BScrollView* scrollView = new BScrollView("scrollview",
		fSettingsTypeListView, B_FRAME_EVENTS | B_WILL_DRAW, false, true);

	BButton *buttonCancel = new BButton("Cancel", B_TRANSLATE("Cancel"),
		new BMessage(B_QUIT_REQUESTED));

	BButton *buttonSave = new BButton("Save", B_TRANSLATE("Save"),
		new BMessage('apla'));

	BBox *fLabelBox = new BBox("SettingsContainerBox2");
	fSettingsContainerBox = new BView("SettingsContainerBox",0);
	fSettingsContainerBox->SetLayout(new BCardLayout());
	fLabelBox->AddChild(fSettingsContainerBox);

	BLayoutBuilder::Group<>(this)
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
			.Add(scrollView)
			.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
				//.Add(fSettingsContainerBox)
				.Add(fLabelBox)
				.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
					.AddGlue()
					.Add(buttonCancel)
					.Add(buttonSave)
				.End()
			.End()
		.SetInsets(B_USE_WINDOW_SPACING)
		.End();

}



void	
SettingsWindow::Select(int i){
	if(fSettingsTypeListView->CurrentSelection()!=i)
			fSettingsTypeListView->Select(i);
	((BCardLayout*) fSettingsContainerBox->GetLayout())->SetVisibleItem(i);
	//fLabelBox->SetLabel(dynamic_cast<BStringItem*>(fSettingsTypeListView->ItemAt(i))->Text());

}
 					
void	
SettingsWindow::AddBox(const char* name, PBox* box){
	box->GroupLayout()->SetInsets(B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING,B_USE_WINDOW_SPACING,B_USE_WINDOW_SPACING);
	fSettingsTypeListView->AddItem(new BStringItem(name));
	// constraint the listview width so that the longest item fits
	float width = 0;
	fSettingsTypeListView->GetPreferredSize(&width, NULL);
	width += B_V_SCROLL_BAR_WIDTH;
	fSettingsTypeListView->SetExplicitMinSize(BSize(width, 0));
	fSettingsTypeListView->SetExplicitMaxSize(BSize(width, B_SIZE_UNLIMITED));
	((BCardLayout*) fSettingsContainerBox->GetLayout())->AddView(box);
}


bool
SettingsWindow::QuitRequested(){
	return true;
}


void
SettingsWindow::SaveSettings(){

	for(int i=0;i<((BCardLayout*) fSettingsContainerBox->GetLayout())->CountItems();i++){
			PBox* pb = dynamic_cast<PBox*>(((BCardLayout*) fSettingsContainerBox->GetLayout())->ItemAt(i)->View());
			
			if(pb)
			{
				BMessage data;
				pb->GetData(&data);
				GetSetting()->RemoveData(pb->GetSettingsName());
				GetSetting()->AddMessage(pb->GetSettingsName(),&data);	
			}	
		}
}

void	
SettingsWindow::MessageReceived(BMessage *mess){
		switch(mess->what) {
		
			case 'list':
				Select(mess->FindInt32("index"));
			break;
			
			case 'apla':
					SaveSettings();
					if(fHandler)
						BMessenger(fHandler).SendMessage(fApplyWhatMsg);
					Quit();
			break;
			default:
				BWindow::MessageReceived(mess);
			break;	
	}		
}



