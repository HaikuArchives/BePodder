#include "SectionSelector.h"
#include "Utils.h"

#define CURVE		2

#define NUM_PART	4
#define PART_SIZE	100.

#define	BORDER		0
#define	TABSIZE		20
#define	W			41
#define	TAB_SPACE	3
#define	TOP_START	0
#define BOTTOM_SPACE 3

#define kTEXT_MARGIN 8

const rgb_color back = ui_color(B_PANEL_BACKGROUND_COLOR); //{ 255,255,255,255};
const rgb_color line = {  255,  255,  255,  255};
const rgb_color sel  = { 242,227,  0,255};

class SmartSelector : public BView {

	public:
		SmartSelector(BRect rect,BBitmap* icon,SectionSelector* boss,int id):BView(rect,"",B_FOLLOW_NONE,B_WILL_DRAW){
			fPressed = false;
			fIcon=icon;
			fBoss = boss;
			fId = id;
			SetViewColor(B_TRANSPARENT_COLOR);
		}
		
		void Draw(BRect r){
			
			if(fPressed)
				SetHighColor(sel);
			else
				SetHighColor(line);
		
			BRect rect(Bounds());
			//rect.InsetBy(1,1);
			FillRect(rect);
	
			if(fIcon){
				SetDrawingMode(B_OP_ALPHA);
				DrawBitmap(fIcon);
			}
		}		

		void SetPressed(bool value){
			fPressed = value;
			Invalidate();
		}

		void MouseUp(BPoint p){

			if(!fPressed){	
				fBoss->Select(fId);
				fBoss->Invoke();
			}
			
		}
		
	private:
		bool fPressed;
		BBitmap*	fIcon;
		SectionSelector* fBoss;
		int fId;

};



SectionSelector::SectionSelector(BRect r,BMessage* msg):BControl(r,"","",msg,B_FOLLOW_ALL_SIDES,B_WILL_DRAW){

	fSelected = -1;
	width = r.right;
	//sel = tint_color(back,B_DARKEN_4_TINT);	
	//SetViewColor(B_TRANSPARENT_COLOR);
	count=0;
}


void
SectionSelector::AddSection(const char* iconName,BView* view,const char* tooltip){
	if(count < MAX_ITEM){
		Section *sec=new Section;
		
		sec->view = view;
				
		float prevPos = count * (W+TAB_SPACE);
		
		float width = W;
		
		BRect rect = BRect(prevPos,
					 	   TOP_START, 
					 	   prevPos + W - 1,
					 	   TOP_START + TABSIZE - 1);
	
		
		sec->tab = new SmartSelector(rect,LoadIcon(iconName),this,count);
		#ifdef ZETA
		 sec->tab->SetToolTipText(tooltip);
		#endif
		
		AddChild(sec->tab);
		
		
		fStack[count++]=sec;
		
		BRect b(Bounds());
		
		view->MoveTo(BORDER,TABSIZE + BORDER + BOTTOM_SPACE);
		
		AddChild(view);
		view->ResizeTo(b.right - 2*BORDER,b.bottom- 2*BORDER - TABSIZE - BOTTOM_SPACE);
		
		view->Hide();
	}		
}


void
SectionSelector::Draw(BRect r){

	//if( ! r.Intersects( BRect(0,0,Bounds().right,TABSIZE) ) return;
		
	//BControl::Draw(r);	
	//for(int i=0;i<count;i++)
	//	DrawPart(i);
		
	BControl::Draw(r);
	
}

/*
void	
SectionSelector::DrawPart(int i){
	
	if(count >= MAX_ITEM) return;
	
	BRect r(fStack[i]->rect);
	//SetHighColor(back);
	//FillRect(r);
	
	
	if(i==fSelected){
		SetHighColor(sel);
	}
	else
	{
		SetHighColor(line);
	}
	
	BRect newR(0,0,W - 2  ,TABSIZE - 2);
	newR.OffsetTo(r.left + GAP_TAB + TAB_SPACE + 1,r.top + TOP_START + 1);
	FillRect(newR);
	
		
	
		
	//SetHighColor(line);
	//DrawString(fStack[i]->name.String(),this,r);
	if(fStack[i]->icon){
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(fStack[i]->icon,BPoint(r.left + GAP_TAB + TAB_SPACE ,r.top + TOP_START));
	}
}

*/

void
SectionSelector::Select(int index){

	if(index == fSelected) return;
	
	int oldsel=fSelected;
	
	fSelected =index;
	
	if(oldsel>=0) {
		fStack[oldsel]->view->Hide();
		((SmartSelector*)fStack[oldsel]->tab)->SetPressed(false);
	}
	
	fStack[index]->view->Show();
	((SmartSelector*)fStack[index]->tab)->SetPressed(true);
	

}
/*void 
SectionSelector::MouseUp(BPoint p){

	
	for(int i=0;i<count;i++){
	
		if(fStack[i]->rect.Contains(p))
		{
			if(i != fSelected){	
				Select(i);
				Invoke();
			}
			return;
		}
		
	}
}

*/


void 
SectionSelector::DrawString(const char* string, BView* parent, BRect rect)
{
	float		width = rect.Width() - (2 * kTEXT_MARGIN);
	float		y;
	BFont font;
	font_height	finfo;

	GetFont(&font);
	font.GetHeight(&finfo);
	
	y = rect.top + ((rect.Height() - (finfo.ascent + finfo.descent + finfo.leading)) / 2) +
					(finfo.ascent + finfo.descent) - 2;

/*	switch (Alignment())
	{
		case B_ALIGN_LEFT:
			parent->MovePenTo(rect.left + kTEXT_MARGIN, y);
			break;

		case B_ALIGN_CENTER:*/
			parent->MovePenTo(rect.left + kTEXT_MARGIN + ((width - font.StringWidth(string)) / 2), y);
	/*		break;

		case B_ALIGN_RIGHT:
			parent->MovePenTo(rect.right - kTEXT_MARGIN - font.StringWidth(string), y);
			break;
	}*/
	parent->DrawString(string);
}

//..
