
#include "AddWindow.h"
#include "Application.h"
#include <LayoutBuilder.h>
#include <Message.h>
#include <Catalog.h>
#include <Clipboard.h>
#include <Box.h>
#include <Button.h>
#include <StringView.h>
#include <View.h>
#include <ScrollView.h>
#include "MainWindow.h"
#include "BPAlert.h"
#include "PBox.h"

extern MainWindow*	main_window;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "AddWindow"
 
AddWindow::AddWindow() :
	BWindow(BRect(250,70,640,270),"AddWindow", B_MODAL_WINDOW, B_NOT_RESIZABLE
	| B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{

	BMessage templ;
	templ.AddString("name", B_TRANSLATE("Add subscription"));
	BMessage settings;
	settings.AddString("name", "rss_url");
	settings.AddString("description", B_TRANSLATE("Feed URL"));
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
				.Add(new BButton("", B_TRANSLATE("Cancel"),new BMessage('can')))
				.Add(new BButton("", B_TRANSLATE("Add"),new BMessage('sav')))
	.End();

	// Check if the clipboard contains a valid url
	const char *clip_text = NULL;
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
					url.RemoveAll("\n");
					url.RemoveAll("\t");
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
		textView->DisallowChar('\n');
		textView->DisallowChar('\t');
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
		


