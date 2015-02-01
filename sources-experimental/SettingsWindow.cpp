#include "SettingsWindow.h" 
#include "SettingsTextView.h"

#include <ListView.h> 
#include <Message.h>
#include "BPLocale.h"
#include <StorageKit.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <StringView.h>
#include "PBox.h"
#include <Alert.h>



SettingsWindow::SettingsWindow(BMessage* setting,BHandler* handler,int32 applyMsg) :
	BWindow(BRect(250,100,750,350),_T("Preferences"), B_FLOATING_WINDOW_LOOK,B_MODAL_APP_WINDOW_FEEL,B_NOT_RESIZABLE| B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS)
{
	fApplyWhatMsg = applyMsg;
	fSetting = setting;
	fHandler = handler;
	fSelected = NULL;
  	
  	rgb_color bgcolor = ui_color(B_PANEL_BACKGROUND_COLOR);
  
	BBox	*bgbox=new BBox(Bounds(),"back",B_FOLLOW_ALL,B_WILL_DRAW,B_PLAIN_BORDER);
	AddChild(bgbox);
	
	BRect rect, arect, brect ;
	
	rect = Bounds();
	rect.InsetBy(5, 5);
	rect.bottom -= 50;
	
	bgView=new BView(BRect(rect),NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	bgView->SetViewColor(bgcolor); //255,0,0);
	bgbox->AddChild(bgView);	
	
	arect = rect;
	arect.right = arect.left + 100;
	arect.bottom = rect.bottom - 10;
	slist=new BListView(arect,"preferences_list",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW);

	slist->SetSelectionMessage(new BMessage('list'));
	BScrollView *scroll=new BScrollView("scrolling", slist, B_FOLLOW_ALL, 0, false, false);
	bgView->AddChild(scroll);
	slist->Select(0);
	slist->MakeFocus(true);
	
	brect=Bounds();
	brect.InsetBy(5, 5);
	brect.top = rect.bottom + 10;
	
	BView *bottomView=new BView(brect,NULL, B_FOLLOW_BOTTOM, B_WILL_DRAW);
	bottomView->SetViewColor(bgcolor);
	bgbox->AddChild(bottomView);
	
	// eliminare bottone close ----------------
	
	BFont	font(be_plain_font);
	arect = bottomView->Bounds();
	arect.InsetBy(10, 10);
	arect.top = arect.bottom - 20;
	arect.left = arect.right - font.StringWidth(_T("Cancel")) - 30;
	BButton *apply = new BButton(arect, "", _T("Save"), new BMessage('apla'));
	bottomView->AddChild(apply);
	
	
	
	arect.right = arect.left - 20;
	arect.top = arect.bottom - 20;
	arect.left = arect.right - font.StringWidth(_T("Save")) - 30;

	BButton *close = new BButton(arect, "", _T("Cancel"), new BMessage(B_QUIT_REQUESTED));
	bottomView->AddChild(close);
	
	
	dxrect = bgView->Bounds();
	
	trect = dxrect;
	trect.InsetBy(0,5);
	trect.left += 5;
	trect.right -= 123;
	trect.top += 1;
	trect.bottom -= 135;
	
	dxrect.top += 2;
	dxrect.bottom -= 2;
	dxrect.left += 115;
	dxrect.right -= 2;
	

	
	
	MoveTo(BAlert::AlertPosition(Bounds().Width(),Bounds().Height()));
}





void	
SettingsWindow::Select(int i){
	
		if(fSelected)
				fSelected->Hide();
				
						
		BBox* newb=fBoxList.ItemAt(i);
		
		if(newb){
			newb->Show();
			fSelected=newb;
		}
		else
			fSelected = NULL;		

		if(slist->CurrentSelection()!=i)
			slist->Select(i);
}
 					
void	
SettingsWindow::AddBox(const char* name, BBox* box){
	box->ResizeTo(dxrect.Width(),dxrect.Height());
	box->MoveTo(dxrect.left,dxrect.top);
	slist->AddItem(new BStringItem(name));
	bgView->AddChild(box);
	box->Hide();
	fBoxList.AddItem(box);
}


bool
SettingsWindow::QuitRequested(){
	return true;
}


void
SettingsWindow::SaveSettings(){
		for(int i=0;i<fBoxList.CountItems();i++){
			PBox* pb = dynamic_cast<PBox*>(fBoxList.ItemAt(i));
			
			if(pb)
			{
				BMessage data;
				pb->GetData(&data);
				//data.PrintToStream();
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



