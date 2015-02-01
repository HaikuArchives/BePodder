#include "BoxRenderer.h"

InstanceBox*	
BoxRenderer::CreateInstanceBox(BRect preferredSize,BString txt){
	InstanceBox *box=new InstanceBox(preferredSize,txt,this);
	return box;
}

bool			
BoxRenderer::HasAction(InstanceBox* ibox,BPoint p,BString* action) {
	if(ibox->action != "" && ibox->rect.Contains(p)) {
		if(action) action->SetTo(ibox->action);
		return true;
	}
	else
	return false;
}

void	
BoxRenderer::HAlignBox(InstanceBox* box,float maxwidth){

	alignment ali=box->box_alignment;

	switch (ali)
	{
		case B_ALIGN_LEFT:
		break;

		case B_ALIGN_CENTER:
		{
			float diff = (maxwidth - box->rect.Width()) / 2.0;
			if(diff>0)
				box->rect.OffsetBy(diff,0);
			break;
		}
		case B_ALIGN_RIGHT:
		{
			float diff = (maxwidth - box->rect.right);
			if(diff>0)
				box->rect.OffsetTo(diff, box->rect.top);
		}
		break;
	}
}
