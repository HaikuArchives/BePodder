
#include "AddWindow.h"
#include "Application.h"

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
	BWindow(BRect(250,70,640,270),"AddWindow", B_MODAL_WINDOW, B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS)
{

	
	BButton*		but;
	BButton*		but2;
	
	BFont font=*(be_plain_font);
	

	BRect r(Bounds());
	
	BBox *box = new BBox(r,"url_bbox", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW, B_FANCY_BORDER);
	box->SetBorder(B_PLAIN_BORDER);
	
	AddChild(box);
	BRect	rect(0, 0, 390, 165);
	
	BMessage templ;
	templ.AddString("name", "Add Subscription");
	BMessage settings;
	settings.AddString("name", "rss_url");
	settings.AddString("description", "Feed URL");
	settings.AddInt32("type", B_STRING_TYPE);
	settings.AddBool("multi_line", true);
	templ.AddMessage("setting", &settings);
	BMessage values;

	text = new PBox(rect, templ, values, "rss_url");
	text->SetBorder(B_PLAIN_BORDER);
	
	box->AddChild(text);
	
	
	
	addrect = box->Bounds();
	addrect.InsetBy(25, 20);
	addrect.top = addrect.bottom - 12;
	addrect.left = addrect.right - font.StringWidth(_T("Cancel")) - 30;
	box->AddChild(but = new BButton(addrect,"",_T("Cancel"),new BMessage('can')));
	
	addrect.right = addrect.left - 15;
	addrect.left = addrect.right - font.StringWidth(_T("Add")) - 30;
	box->AddChild(but2 = new BButton(addrect,"",_T("Add"),new BMessage('sav')));


	//controlliamo se c'e' qualcosa nella clipboard?
	const char *clip_text = NULL;
	bool	addButton = false;
	int32 textLen;
	BString url;
	BMessage *clip = (BMessage *)NULL;
	if (be_clipboard->Lock() ){
		clip = be_clipboard->Data();
		
		if ( clip && ( clip->FindData("text/plain", B_MIME_TYPE,(const void **)&clip_text, &textLen) == B_OK )){
		url.SetTo(clip_text,textLen);
		
		if(url.FindFirst("http://") == 0)
			addButton = true;
		else
		if(url.FindFirst("pcast://") == 0)
			addButton = true;
		else
		if(url.FindFirst("feed://") == 0)
			addButton = true;
		}	
		be_clipboard->Unlock();
	} 
	
	if(addButton==true){
		addrect.right = addrect.left - 15;
		addrect.left = addrect.right - font.StringWidth(_T("Paste")) - 30;
		BMessage* msg=new BMessage('past');
		msg->AddString("url",url);
		BButton* clip_but;
		box->AddChild(clip_but = new BButton(addrect,"",_T("Paste"),msg));
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
	else
	if(mess->what== 'past'){
		BString url;
		mess->FindString("url",&url);
		BTextView *textView = (BTextView*)FindView("rss_url");
		if (textView){
			textView->SetText(url.String());
		}
	}
	else BWindow::MessageReceived(mess);
}
		


