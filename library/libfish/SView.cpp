#include "SView.h"
#include "Application.h"
#include "Bitmap.h"
#include "TranslationUtils.h"

#include "stdio.h"

#define DEBUG 0

SView::SView(BRect r):BView(r,"",B_FOLLOW_NONE,B_WILL_DRAW){
	fDoc=NULL;
	fCompositeRenderer = new CompositeRenderer();
	fMouseDown = false;
}

void	
SView::DisplayDocument(HDoc* doc){

	fDoc=doc;
	if(!fDoc) return;
	
	ResizeTo(Bounds().Width(),fDoc->GetHeight());
	SetViewColor(fDoc->backColor);
	SetHighColor(fDoc->foreColor);
	SetLowColor(fDoc->backColor);
	Invalidate();
}

void
SView::Draw(BRect r)
{	
	if(fDoc==NULL) return;
	
	if(DEBUG)
		StrokeRect(BRect(0,0,fDoc->GetWidth(),fDoc->GetHeight()));
	
	fDoc->renderer->DrawBox(this,fDoc);	
	//fCompositeRenderer->DrawBox(this,fDoc);	
}

void	
SView::RegisterBoxRenderer(int id,BoxRenderer* renderer){
	fRenderers.AddItem(id,renderer);
}

void	
SView::UnregisterBoxRenderer(int id){
	fRenderers.RemoveItemFor(id);
}
 
BoxRenderer*	
SView::GetRenderer(int id){
	return fRenderers.ValueFor(id);
}



void		
SView::GetPreferredSize(float*w,float*h){
	*w=10;
	*h=10;
}	 

void
SView::AttachedToWindow(){
	SetViewColor(255,0,255,255);
}	 

//..
