#include "HelpWindow.h"


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <Box.h>
#include <Button.h>
#include <Catalog.h>
#include <Application.h>

#include "libfish/HelpViewer.h"
#include "Utils.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "HelpWindow"

HelpWindow::HelpWindow() :
	BWindow(BRect(300,100,780,475),B_TRANSLATE("Getting started"), B_TITLED_WINDOW,B_NOT_RESIZABLE| B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS|B_WILL_DRAW){
	
	BRect rect(Bounds());
	rect.bottom *= 0.87;
	
	AddChild(hv=new HelpViewer(rect));
	
	rect.top = rect.bottom+1;
	rect.bottom = Bounds().bottom;
	BBox *box;
	AddChild(box=new BBox(rect,""));
	
	BFont font(be_plain_font);
	
	
	BRect arect;
	arect = box->Bounds();
	arect.InsetBy(33, 20);
	arect.top = arect.bottom - 17;
	arect.left = arect.right - font.StringWidth(B_TRANSLATE("Next")) - 35;   //anche qui va il _T() altrimenti considera sempre la dimensione di Next e non della stringa tradotta
	next = new BButton(arect, "", B_TRANSLATE("Next"), new BMessage('next'));
	next->SetFont(&font);
	box->AddChild(next);
	
	arect.right = arect.left - 17;
	arect.left = arect.right - font.StringWidth(B_TRANSLATE("Prev")) - 35;
	back= new BButton(arect, "", B_TRANSLATE("Prev"), new BMessage('prev'));
	back->SetFont(&font);
	box->AddChild(back);
	
	arect = box->Bounds();
	arect.InsetBy(33, 20);
	arect.top = arect.bottom - 17;
	arect.right = arect.left  + font.StringWidth(B_TRANSLATE("Index")) + 35;
	BButton *index= new BButton(arect, "", B_TRANSLATE("Index"), new BMessage('inde'));
	box->AddChild(index);
	
	
	rgb_color black = {0,0,0,255};
	rgb_color white = {255,255,255,255};
	hv->SetForeColor(black) ;
	hv->SetBackColor(white);
		
	
	BString where(GetAppRelativePath());
	#ifdef ZETA
		where << "/help.xml";
	#else
		where << "/helpR5.xml";
	#endif
	hv->LoadFile(where.String());
	hv->SetGotoHandler(this);
	hv->SetOpenUrlHandler(be_app);
	
	Selected = -1;
	SetPage(0);
	
}


void	
HelpWindow::SetPage(BString id){
	if(Lock()){
		hv->SearchSetPage(id);
	Unlock();
	}
}
	
void	
HelpWindow::SetPage(int newindex){
	
	if(Selected == newindex) return;
	if(newindex >= hv->CountPage()) return;
	
	Selected = newindex;
	hv->SetPage(Selected);
	
	next->SetEnabled(true);
	back->SetEnabled(true);
	
	if(Selected ==  hv->CountPage() - 1)
	{
		
		next->SetEnabled(false);
	}
	
	if(Selected ==  0)
	{
		back->SetEnabled(false);
	}
	
}
		
/*
bool
HelpWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
*/
void
HelpWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case 'inde':
			SetPage(BString("index"));
		break;
		case 'next':
			SetPage(Selected+1);
		break;
		case 'prev':
			SetPage(Selected-1);
		break;
		case HELPVIEWER_GOTO:
		{
			
			int32 newindex;
			if(message->FindInt32("selected",&newindex)==B_OK){
				
				if(Selected == newindex) return;
				if(newindex >= hv->CountPage()) return;
				Selected = newindex;
				
				
				next->SetEnabled(true);
				back->SetEnabled(true);
				if(Selected ==  hv->CountPage() - 1)
				{
					next->SetEnabled(false);
				}
				if(Selected ==  0)
				{
					back->SetEnabled(false);
				}
			}
			
		}
		break;
		default:
		BWindow::MessageReceived(message);
		break;
	
	}	
}

//.
