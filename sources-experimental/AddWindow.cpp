
#include "AddWindow.h"
#include "Application.h"
#include <LayoutBuilder.h>
#include <Message.h>
#include "BPLocale.h"
#include <Button.h>
#include <StringView.h>
#include <Box.h>
#include <StringView.h>
#include <View.h>
#include <Clipboard.h>
#include <ScrollView.h>
#include "MainWindow.h"
#include "BPAlert.h"
#include "PBox.h"

extern MainWindow*	main_window;

 
AddWindow::AddWindow() :
	BWindow(BRect(250,70,640,270),"AddWindow", B_MODAL_WINDOW, B_NOT_RESIZABLE
	| B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{

	BMessage templ;
	templ.AddString("name", "Add subscription");
	BMessage settings;
	settings.AddString("name", "rss_url");
	settings.AddString("description", "Feed URL");
	settings.AddInt32("type", B_STRING_TYPE);
	settings.AddBool("multi_line", true);
	templ.AddMessage("setting", &settings);
	BMessage values;
	BRect rect(0, 0, 390, 165);
	text = new PBox(rect, templ, values, "rss_url");
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
				B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING)
			.Add(text)
		.AddGroup(B_HORIZONTAL, B_USE_ITEM_SPACING)
				.AddGlue()
				.Add(new BButton("",_T("Cancel"),new BMessage('can')))
				.Add(new BButton("",_T("Add"),new BMessage('sav')))

	.End();

	//controlliamo se c'e' qualcosa nella clipboard?
	const char *clip_text = NULL;
	bool	addButton = false;
	ssize_t textLen;
	BString url;
	BMessage *clip = (BMessage *)NULL;
	if (be_clipboard->Lock() ){
		clip = be_clipboard->Data();
		
		if ( clip && ( clip->FindData("text/plain", B_MIME_TYPE,(const void **)&clip_text, &textLen) == B_OK )) {
			url.SetTo(clip_text,textLen);
		
			if(url.FindFirst("http://") == 0
				||  url.FindFirst("https://") == 0
				||  url.FindFirst("pcast://") == 0
				||  url.FindFirst("feed://") == 0)
			{
				BTextView *textView = (BTextView*)FindView("rss_url");
				if (textView){
					textView->SetText(url.String());
					textView->SelectAll();
				}
			}
			be_clipboard->Unlock();
		}
	}

	MoveTo(BPAlert::AlertPosition(Bounds().Width(),Bounds().Height()));
	
	
	BTextView *textView = (BTextView*)FindView("rss_url");
	if (textView){
		textView->MakeFocus(true);
	}
}


void	
AddWindow::MessageReceived(BMessage *mess){

	
	if(mess->what == 'can'){
		BMessenger(this).SendMessage(B_QUIT_REQUESTED); 
	}
	else
	if(mess->what == 'sav'){
		BTextView *textView = (BTextView*)FindView("rss_url");
		if (textView){
			BMessage	msg(DOWNLOAD_CHANNEL);
			msg.AddString("url",textView->Text());
			BMessenger(main_window).SendMessage(&msg);
			BMessenger(this).SendMessage(B_QUIT_REQUESTED);
		}
	}
	else BWindow::MessageReceived(mess);
}
		


