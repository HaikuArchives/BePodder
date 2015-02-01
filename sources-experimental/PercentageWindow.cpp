#include "PercentageWindow.h"

#include "StripeView.h"
#include "BPAlert.h"
#include "PercentageView.h"

#include <StringView.h>
#include <Messenger.h>

#define DEFAULT_RECT	BRect(0, 0, 310, 75)

static const int kTextIconOffsetSpace = 30;

PercentageWindow::PercentageWindow(const char* title,const char* text,BBitmap* icon) : BWindow(DEFAULT_RECT, title, B_MODAL_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE){

		// Set up the "_master_" view
		
	StripeView* masterView = new StripeView(Bounds());
	AddChild(masterView);
	masterView->SetBitmap(icon);
	
	kTextIconOffset = 0;
	
	if(masterView->Bitmap())
		kTextIconOffset = masterView->Bitmap()->Bounds().right + kTextIconOffsetSpace;


	//ok, un String
	//il percentage (a 0) con testo percentuale
	float maxW;
	
	BStringView*		string=new BStringView(BRect(kTextIconOffset,6,0,0),"_text",text);
	masterView->AddChild(string);
	string->ResizeToPreferred();
	maxW=string->Frame().right + 6;
	
	BRect rect(string->Frame());
	
	rect.top=string->Frame().bottom+6;
	
	pw=new PercentageView(rect);
	masterView->AddChild(pw);
	pw->ResizeToPreferred();
	
	rect=pw->Frame();
	rect.left=rect.right+6;
	
	
	perc=new BStringView(rect,"_percentage","100%");
	masterView->AddChild(perc);
	perc->ResizeToPreferred();
	if(perc->Frame().right + 6 > maxW)
			maxW=perc->Frame().right + 6;
	
	perc->SetText("0%");	
	
	maxW +=kTextIconOffsetSpace;
		
	//if(maxW>Bounds().right) 
	ResizeTo(maxW,Bounds().bottom);	
		
	SetLook(B_FLOATING_WINDOW_LOOK);
		
	MoveTo(BPAlert::AlertPosition(Frame().Width(), Frame().Height()));	
}

void
PercentageWindow::SetPercentage(int p){

	
	BString text;
	text << p << "%";
	
	if(Lock()){
		perc->SetText(text.String());	
		pw->SetPercentage(p);
		Unlock();
	}
	
}

bool		
PercentageWindow::QuitRequested(){
	if(fLooper)
		BMessenger(fLooper).SendMessage(fMsg);
	
	return true;
}


void		
PercentageWindow::Go(BLooper* lop,int32 msg){
	fLooper = lop;
	fMsg=msg;
	Show();
}
