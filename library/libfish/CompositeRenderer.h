#ifndef CompositeRenderer_H_
#define CompositeRenderer_H_

#include <View.h>
#include "BoxRenderer.h"
#include "ContainerBox.h"

class CompositeRenderer : public BoxRenderer {

	public:
	
	InstanceBox*	CreateInstanceBox(BRect preferredSize,BString txt);
	
	void			DrawBox(BView* parent,InstanceBox* ibox);
	
	void			InitInstanceBox(InstanceBox *,ContainerBox *);	
	void			FreeInstanceBox(InstanceBox*);

	bool			HasAction(InstanceBox* ibox,BPoint p,BString* action=NULL);
};
#endif
//--
