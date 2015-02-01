#include "PBox.h"
#include <stdlib.h>

#include <Entry.h>
#include <Roster.h>
#include <ScrollView.h>
#include <Application.h>
#include <Alert.h>
#include <stdio.h>

#include "SettingsTextView.h"


#define _T(A) A

const float kControlOffset = 6.0;
const float kEdgeOffset = 5.0;
const float kDividerWidth = 100;



#define SCROLLBAR 0

PBox::PBox(BRect rect, BMessage fT, BMessage fD,const char* settings_name)
	: BBox(rect, "Property", B_FOLLOW_ALL,B_WILL_DRAW) 
{

	fSName.SetTo(settings_name);
	fTemplate = fT;
	fData = fD;

#if B_BEOS_VERSION > B_BEOS_VERSION_5
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(ui_color(B_PANEL_TEXT_COLOR));
#else
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(0, 0, 0, 0);
#endif

};

void
PBox::AttachedToWindow(){
	
	//calc font height (for nice control size)	
	font_height fontHeight;
	be_bold_font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.descent + fontHeight.leading + fontHeight.ascent;


	BRect frame = Bounds();
	frame.InsetBy(5,5);
	
	BRect temp = frame;
	temp.bottom = temp.top + 40;	
	
	SettingsTextView *testo1=new SettingsTextView(temp,"testo",_T("Settings"));
	AddChild(testo1);
	frame.top +=40;
	//BSeparator *sep=new BSeparator(frame,"sep","cippa");
	//sep->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	//AddChild(sep);

	//frame.InsetBy(kEdgeOffset, kEdgeOffset);
	//frame.top += fFontHeight; //some space

#if SCROLLBAR
	frame.right -= B_V_SCROLL_BAR_WIDTH + 2;
#endif
	
			BView *view = new BView(frame, "", B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
						
#if B_BEOS_VERSION > B_BEOS_VERSION_5
			view->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			view->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			view->SetHighColor(ui_color(B_PANEL_TEXT_COLOR));
#else
			view->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			view->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			view->SetHighColor(0, 0, 0, 0);
#endif

	
			float settings_height = BuildGUI(fTemplate, fData, view);
					

			view->ResizeTo( view->Bounds().Width(), settings_height);
#if	SCROLLBAR			


			BScrollView * scroller = new BScrollView(
				"scroller", view, B_FOLLOW_ALL,
				0, false, true
			);
			float scroll_height = scroller->Bounds().Height();
			float diff = settings_height - scroll_height;
			if ( diff < 0 ) diff = 0;
			
			scroller->ScrollBar(B_VERTICAL)->SetRange(0, diff);

			view = scroller;
#endif
			fPrefView = view;

			AddChild(fPrefView);
			
			testo1->SetText(fTemplate.FindString("name"));
}

void
PBox::GetData(BMessage* data){
			
			
			
			BView * panel = fPrefView;
			BMessage cur;
			
			for (int i = 0; fTemplate.FindMessage("setting", i, &cur) == B_OK; i++) {
				const char *name = cur.FindString("name");
				int32 type = -1;
				
				cur.FindInt32("type", &type);
				
				if ( dynamic_cast<BTextControl*>(panel->FindView(name))) { 
//					Free text
					BTextControl * ctrl = (BTextControl*)panel->FindView(name);
				
					switch (type) {
						case B_STRING_TYPE: {
							data->AddString(name, ctrl->Text() );
						} break;
						case B_INT32_TYPE: {
							data->AddInt32(name, atoi(ctrl->Text()) );
						} break;
						default: {
							return;
						};
					};
				} else if (dynamic_cast<BMenuField*>(panel->FindView(name))) {
//					Provided option
					BMenuField * ctrl = (BMenuField*)panel->FindView(name);
					BMenuItem * item = ctrl->Menu()->FindMarked();
					
									
					if (!item) return;
					
					switch (type) {
						case B_STRING_TYPE: {
							data->AddString(name, item->Label() );
							BString index(name);
							index << "_index";
							data->AddInt32(index.String(), ctrl->Menu()->IndexOf(item)); //index 
						} break;
						case  B_INT32_TYPE: {
							data->AddInt32(name, atoi(item->Label()) );
							BString index(name);
							index << "_index";
							data->AddInt32(index.String(), ctrl->Menu()->IndexOf(item)); //index
						} break;
						default: {
							return;
						};
					}
				} else
				if (dynamic_cast<BCheckBox*>(panel->FindView(name))) {
// 					Boolean setting
					BCheckBox * box = (BCheckBox*)panel->FindView(name);
					
					if ( box->Value() == B_CONTROL_ON ) {
						data->AddBool(name,true);
					} else {
						data->AddBool(name,false);
					}
				} else if (dynamic_cast<BTextView *>(panel->FindView(name))) {
					BTextView *view = (BTextView *)panel->FindView(name);
					data->AddString(name, view->Text());
				};
				
				
			};

		
}
				
void 
PBox::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		
		case 'enab':
		{
			bool value = (bool)msg->FindInt32("be:value");
			for (int j = 0; msg->FindString("enable_control", j); j++) {
				const char* name=msg->FindString("enable_control", j);
				BView*	viewz=fPrefView->FindView(name);
				if(viewz)
					((BControl*)viewz)->SetEnabled(value);
			}
		}
		break;
		default:
			BBox::MessageReceived(msg);
		break;
	};
};

float PBox::BuildGUI(BMessage viewTemplate, BMessage settings, BView *view) {
	
	BMessage curr;
	float yOffset = kEdgeOffset + kControlOffset;
	float xOffset = 0;
	
	const float kControlWidth = view->Bounds().Width() - (kEdgeOffset * 2);
	
	BMessage postAdded;
	
	for (int i=0; viewTemplate.FindMessage("setting",i,&curr) == B_OK; i++ ) {
		char temp[512];
		
		// get text etc from template
		const char * name = curr.FindString("name");
		const char * desc = curr.FindString("description");
		const char * value = NULL;
		int32 type = -1;
		bool secret = false;
		bool freeText = true;
		bool multiLine = false;
		BView *control = NULL;
		BMenu *menu = NULL;
		BMessage* enabling = NULL;
		
		if ( name != NULL && strcmp(name,"app_sig") == 0 ) {
			// skip app-sig setting
			continue;
		}
		
		if (curr.FindInt32("type", &type) != B_OK) {
			printf("Error getting type for %s, skipping\n", name);
			continue;
		};
		
		switch (type) {
			case B_STRING_TYPE: {
				if (curr.FindString("valid_value")) {
					// It's a "select one of these" setting
					
					freeText = false;
			
					menu = new BPopUpMenu(name);
//					menu->SetDivider(be_plain_font->StringWidth(name) + 10);
					
					for (int j = 0; curr.FindString("valid_value", j); j++) {
						menu->AddItem(new BMenuItem(curr.FindString("valid_value", j),NULL));
					};
					
					value = NULL;
					value = curr.FindString("default");
					if (value && menu->FindItem(value)) menu->FindItem(value)->SetMarked(true);
					
					value = settings.FindString(name);
					if (value && menu->FindItem(value)) menu->FindItem(value)->SetMarked(true);
					
				} else {
					// It's a free-text setting
					
					if (curr.FindBool("multi_line", &multiLine) != B_OK) multiLine = false;
					value = settings.FindString(name);
					if (!value) value = curr.FindString("default");
					if (curr.FindBool("is_secret",&secret) != B_OK) secret = false;
				}
			} break;
			case B_INT32_TYPE: {
				if (curr.FindInt32("valid_value")) {
					// It's a "select one of these" setting
					
					freeText = false;
					
					menu = new BPopUpMenu(name);
					
					int32 v = 0;
					for ( int j = 0; curr.FindInt32("valid_value",j,&v) == B_OK; j++ ) {
						sprintf(temp,"%ld", v);
						menu->AddItem(new BMenuItem(temp, NULL));
					};
					
					if ( curr.FindInt32("default",&v) == B_OK ) {
						sprintf(temp,"%ld",v);
						value = temp;
						menu->FindItem(value)->SetMarked(true);
					}
					
					if ( settings.FindInt32(name,&v) == B_OK ) {
						sprintf(temp,"%ld",v);
						value = temp;
						menu->FindItem(value)->SetMarked(true);
					}
					
					
				} else {
					// It's a free-text (but number) setting
					int32 v = 0;
					if (settings.FindInt32(name,&v) == B_OK) {
						sprintf(temp,"%ld",v);
						value = temp;
					} else if ( curr.FindInt32("default",&v) == B_OK ) {
						sprintf(temp,"%ld",v);
						value = temp;
					}
					if (curr.FindBool("is_secret",&secret) != B_OK) secret = false;
				}
			} break;
			case B_BOOL_TYPE: {
				bool active;
				
				if (settings.FindBool(name, &active) != B_OK) {
					if (curr.FindBool("default", &active) != B_OK) {
						active = false;
					};
				};
				
				for (int j = 0; curr.FindString("enable_control", j); j++) {
						if(!enabling) enabling = new BMessage('enab');
						enabling->AddString("enable_control",curr.FindString("enable_control",j) );
						if(!active) postAdded.AddString("disable",curr.FindString("enable_control", j));
				};
					
				control = new BCheckBox(BRect(0, 0, kControlWidth, fFontHeight),
					name, _T(desc), enabling);
			
				if (active) ((BCheckBox*)control)->SetValue(B_CONTROL_ON);
				
			} break;			
			default: {
				continue;
			};
		};
		
		if (!value) value = "";
		
		if (!control) {
			if (freeText) {
				if (multiLine == false) {
					control = new BTextControl(
						BRect(0, 0, kControlWidth, fFontHeight), name,
						_T(desc), value, NULL);
					if (secret) {
						((BTextControl *)control)->TextView()->HideTyping(true);
						((BTextControl *)control)->SetText(_T(value));
					};
					((BTextControl *)control)->SetDivider(kDividerWidth);
				} else 
				{
					BRect rect;
					BRect textRect;
					if (desc) //andrea: add description only if available.
					{
						BRect labelRect(0, 0, kDividerWidth, fFontHeight);
						BStringView *label = new BStringView(labelRect, "NA", _T(desc),
							B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
						view->AddChild(label);
						label->MoveTo(kEdgeOffset, yOffset);

						rect = BRect(0, 0, kControlWidth - kDividerWidth, fFontHeight * 4);
						rect.right -= B_V_SCROLL_BAR_WIDTH + kEdgeOffset + kControlOffset;
						xOffset = kEdgeOffset + kDividerWidth;
					}
					else
					{
						rect = BRect(0, 0, kControlWidth, fFontHeight * 4);
						rect.right -= B_V_SCROLL_BAR_WIDTH + kControlOffset;
						xOffset = 0;
					}
					
					textRect = rect;
					textRect.InsetBy(kEdgeOffset, kEdgeOffset);
					textRect.OffsetTo(1.0, 1.0);
						
					BTextView *textView = new BTextView(rect, name, textRect,
						B_FOLLOW_ALL_SIDES, B_WILL_DRAW);

					control = new BScrollView("NA", textView, B_FOLLOW_ALL_SIDES,
						B_WILL_DRAW | B_NAVIGABLE, false, true);
					textView->SetText(_T(value));			
				};
			} else {
				control = new BMenuField(BRect(0, 0, kControlWidth, fFontHeight),
					name, _T(desc), menu);
				
				float size=kDividerWidth;
				if(control->StringWidth(_T(desc)) > kDividerWidth)
					size=control->StringWidth(_T(desc)) + 20;
				
				((BMenuField *)control)->SetDivider(size);
			};
		};
		
		
		view->AddChild(control);
		
		if(enabling)
			 ((BCheckBox*)control)->SetTarget(this);
					
		float h, w = 0;
		control->GetPreferredSize(&w, &h);
		
		if (h < control->Bounds().Height()) 
			h = control->Bounds().Height();
			
		control->MoveTo(kEdgeOffset + xOffset, yOffset);
		yOffset += kControlOffset + h ;
		xOffset = 0;
	};

	for (int j = 0; postAdded.FindString("disable", j); j++) 
	{
			const char* name=postAdded.FindString("disable", j);
			BView*	viewz=view->FindView(name);
			if(viewz)
				((BControl*)viewz)->SetEnabled(false);
			
	}
	
	//if ( yOffset < view->Bounds().Height() )
	//	yOffset = view->Bounds().Height();
	
	return yOffset;//view->ResizeTo( view->Bounds().Width(), yOffset 
}
