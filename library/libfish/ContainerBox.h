#ifndef ContainerBox_H_
#define ContainerBox_H_

#include "InstanceBox.h"
#include <ObjectList.h>

class ContainerBox : public InstanceBox {

	public:
			ContainerBox(BRect preferredSize,BString txt,BoxRenderer* render):
			InstanceBox(preferredSize,txt,renderer){ left_margin=top_margin=0.0;}
			
			virtual ~ContainerBox(){
		
			while(fBoxes.CountItems()){
				InstanceBox *box=fBoxes.ItemAt(0);
				box->renderer->FreeInstanceBox(box);
				delete box;
				fBoxes.RemoveItem(box);
			}
		
			}
	

		
			
	virtual void	AddInstanceBox(InstanceBox *box){
				if(!box) return;
				//box->renderer->InitInstanceBox(box,this);
				fBoxes.AddItem(box);
			}
			
	float	GetHeight(){ return rect.Height();}
	float	GetWidth(){ return rect.Width();}
	
	virtual	void			ArrangeBoxPosition(InstanceBox*) {};
	virtual void			InitSize(float)  {};
	virtual	void			UpdateSize(InstanceBox* box){}
	virtual	float			MaxWidth(){ return rect.Width() ; /*by default*/}
	
		//this should be wrapped
		BObjectList<InstanceBox> fBoxes;
		
		float	top_margin;
		float	left_margin;
		
				

};
#endif
