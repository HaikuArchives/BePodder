#include "CompositeRenderer.h"

#define DEBUG 0

InstanceBox*	
CompositeRenderer::CreateInstanceBox(BRect preferredSize,BString txt){
	ContainerBox *box=new ContainerBox(preferredSize,txt,this);
	return (InstanceBox*)box;
}

void
CompositeRenderer::DrawBox(BView* parent,InstanceBox* ibox){
	ContainerBox* doc=(ContainerBox*)ibox;
	int32 count = doc->fBoxes.CountItems();
	BRect r(doc->rect);
	InstanceBox* box;
	for(int i=0;i< count ; i++){
		box = doc->fBoxes.ItemAt(i);
				 
		if(r.Intersects(box->rect)){
			//draw!
			if(DEBUG)
				parent->StrokeRect(box->rect);
			box->renderer->DrawBox(parent,box);
		}
					 
	}
}

bool			
CompositeRenderer::HasAction(InstanceBox* ibox,BPoint p,BString* action){
	ContainerBox* doc=(ContainerBox*)ibox;
	int32 count = doc->fBoxes.CountItems();
	InstanceBox* box;
	for(int i=0;i< count ; i++){
		box = doc->fBoxes.ItemAt(i);
		if(box->renderer->HasAction(box,p,action)){
			return true;
		}
	}
	
	return false;
}

void			
CompositeRenderer::InitInstanceBox(InstanceBox *ibox,ContainerBox* parent){
	
	//devo settare le dimensioni!
	
	//debugger("h");
		
	ContainerBox* doc=(ContainerBox*)ibox;

	doc->InitSize(parent->MaxWidth());
	
	InstanceBox* box;
	int32 count = doc->fBoxes.CountItems();
	
	for(int i=0;i< count ; i++){
		box = doc->fBoxes.ItemAt(i);
		
		//if(box->text.Compare("testContainer") == 0) 	debugger("inner");
				
		doc->ArrangeBoxPosition(box);
			box->renderer->InitInstanceBox(box,doc);
		doc->UpdateSize(box);
		
		box->rect.OffsetBy(doc->rect.left,doc->rect.top);
	}
	
	
}	


void			
CompositeRenderer::FreeInstanceBox(InstanceBox* ibox){
	//no cookie
}
