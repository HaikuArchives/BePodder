#include "PBox.h"
#include <stdlib.h>

#include <Entry.h>
#include <GridView.h>
#include <Roster.h>
#include <ScrollView.h>
#include <SpaceLayoutItem.h>
#include <Application.h>
#include <Alert.h>
#include <stdio.h>


#define _T(A) A

const float kEdgeOffset = 5.0;

#define SCROLLBAR 0

PBox::PBox(BRect rect, BMessage fT, BMessage fD,const char* settings_name)
	: BGroupView("Property", B_VERTICAL, B_USE_WINDOW_SPACING),
	fSName(settings_name),
	fTemplate(fT),
	fData(fD)
{
};

void
PBox::AttachedToWindow(){
	AdoptParentColors();
	BuildGUI(fTemplate, fData, this);
	GroupLayout()->AddItem(BSpaceLayoutItem::CreateGlue());
}


void
PBox::GetData(BMessage* data){
			BMessage cur;
			for (int i = 0; fTemplate.FindMessage("setting", i, &cur) == B_OK; i++) {
				const char *name = cur.FindString("name");
				int32 type = -1;
				
				cur.FindInt32("type", &type);
				
				if ( dynamic_cast<BTextControl*>(FindView(name))) {
//					Free text
					BTextControl * ctrl = (BTextControl*)FindView(name);
				
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
				} else if (dynamic_cast<BMenuField*>(FindView(name))) {
//					Provided option
					BMenuField * ctrl = (BMenuField*)FindView(name);
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
				if (dynamic_cast<BCheckBox*>(FindView(name))) {
// 					Boolean setting
					BCheckBox * box = (BCheckBox*)FindView(name);
					
					if ( box->Value() == B_CONTROL_ON ) {
						data->AddBool(name,true);
					} else {
						data->AddBool(name,false);
					}
				} else if (dynamic_cast<BTextView *>(FindView(name))) {
					BTextView *view = (BTextView *)FindView(name);
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
				BView*	viewz=FindView(name);
				if(viewz)
					((BControl*)viewz)->SetEnabled(value);
			}
		}
		break;
		default:
			BGroupView::MessageReceived(msg);
		break;
	};
};


void
PBox::BuildGUI(BMessage viewTemplate, BMessage settings, BView *view) {
	BMessage curr;
	BStringView *title=new BStringView("title",_T("Settings"));
	title->SetText(fTemplate.FindString("name"));
	title->SetAlignment(B_ALIGN_CENTER);
	title->SetFont(be_bold_font);
	AddChild(title);

	BMessage postAdded;
	
	BGridView* gridView = NULL;
	int gridline = 0;

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
					
				control = new BCheckBox(name, _T(desc), enabling);
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
					control = new BTextControl(name, _T(desc), value, NULL);
					if (secret) {
						((BTextControl *)control)->TextView()->HideTyping(true);
						((BTextControl *)control)->SetText(_T(value));
					};
					if (gridView == NULL) {
						gridView = new BGridView();
						view->AddChild(gridView);
					}
					gridView->GridLayout()->AddItem(((BTextControl *)control)->CreateLabelLayoutItem(), 0, gridline);
					gridView->GridLayout()->AddItem(((BTextControl *)control)->CreateTextViewLayoutItem(), 1, gridline);
					gridline++;
				} else 
				{
					BGroupView* groupView = new BGroupView(B_HORIZONTAL);
					BRect rect(0,0,280,80);
					BRect textRect;
					if (desc) //andrea: add description only if available.
					{
						BStringView *label = new BStringView("NA", _T(desc));
						groupView->AddChild(label);
					}
					
					textRect = rect;
					textRect.InsetBy(kEdgeOffset, kEdgeOffset);
					textRect.OffsetTo(1.0, 1.0);
						
					BTextView *textView = new BTextView(rect, name, textRect,
						B_FOLLOW_ALL_SIDES, B_WILL_DRAW);

					textView->SetExplicitMinSize(BSize(300, 80));
					control = new BScrollView("NA", textView, B_FOLLOW_ALL_SIDES,
						B_WILL_DRAW | B_NAVIGABLE, false, true);
					textView->SetText(_T(value));
					groupView->AddChild(control);
					view->AddChild(groupView);
				};
			} else {
				control = new BMenuField(name, _T(desc), menu);
				if (gridView == NULL) {
					gridView = new BGridView();
					view->AddChild(gridView);
				}
				gridView->GridLayout()->AddItem(((BMenuField *)control)->CreateLabelLayoutItem(), 0, gridline);
				gridView->GridLayout()->AddItem(((BMenuField *)control)->CreateMenuBarLayoutItem(), 1, gridline);
				gridline++;
			};
		};


		// Component already added with other layout
		if (dynamic_cast<BMenuField*>(control) == NULL && dynamic_cast<BTextControl*>(control) == NULL && multiLine == false) {
			view->AddChild(control);
			if (gridView != NULL) {
				gridView = NULL;
				gridline = 0;
			}
		}
		
		if(enabling)
			((BCheckBox*)control)->SetTarget(this);
	};

	for (int j = 0; postAdded.FindString("disable", j); j++) 
	{
			const char* name=postAdded.FindString("disable", j);
			BView*	viewz=view->FindView(name);
			if(viewz)
				((BControl*)viewz)->SetEnabled(false);
			
	}
}
