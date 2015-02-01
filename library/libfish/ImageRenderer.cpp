#include "Utils.h"
#include "ImageRenderer.h"
#include "ImageCache.h"

struct token {
	BBitmap*	image;
	BPoint		shift;
};
void
ImageRenderer::DrawBox(BView* parent,InstanceBox* ibox) {
	token *tok=(token*)ibox->cookie;
	if(!tok->image) return;
	parent->SetDrawingMode( B_OP_ALPHA );
	parent->DrawBitmap(tok->image,BPoint(ibox->rect.left,ibox->rect.top)+tok->shift);
	parent->SetDrawingMode( B_OP_OVER );
}

void	
ImageRenderer::InitInstanceBox(InstanceBox *ibox,ContainerBox *){
	
	BBitmap *icon=LoadIcon(ibox->text.String());
	token *tok=new token;
	ibox->cookie=(void*)tok;
	tok->image=NULL;
	tok->shift=BPoint(0,0);
				
	if(icon) {
		tok->image=icon;
	}
			
		  float maxwidth=ibox->rect.Width();
	const float mw=maxwidth;
	
	float maxheight=ibox->rect.Height();
	if(!icon) return;
	
	if(icon->Bounds().Width()<maxwidth)
		maxwidth = icon->Bounds().Width();
	
	maxheight = icon->Bounds().Height();
		
	
	//refine size:
	if(ibox->hlimit>0 && ibox->hlimit<mw && ibox->hlimit>maxwidth){
		
		ibox->SetSize(ibox->hlimit,maxheight);
		float diff = (ibox->hlimit -icon->Bounds().Width()) / 2.0;
		if(diff<0) diff=0;
		tok->shift=BPoint(diff,0);
	}
	else
	{
		ibox->SetSize(maxwidth,maxheight);
		HAlignBox(ibox,mw);
	}

}

void	
ImageRenderer::FreeInstanceBox(InstanceBox* box){
	token *tok=(token*)box->cookie;
	if(tok->image)
		ImageCache::DeleteImage(box->text.String());

	delete tok;
} 
	
