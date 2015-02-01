#ifndef BoxRenderer_H_
#define BoxRenderer_H_

#include <View.h>
#include "InstanceBox.h"

class ContainerBox;

class BoxRenderer {

	public:

	virtual ~BoxRenderer(){}
		
	virtual	void			DrawBox(BView* parent,InstanceBox* ibox) = 0;
	virtual	InstanceBox*	CreateInstanceBox(BRect preferredSize,BString txt);
	
	
	
	
	
	virtual bool			HasAction(InstanceBox* ibox,BPoint p,BString* action=NULL);
	//virtual	void	InstanceClicked(InstanceBox* ibox,BPoint abs){};
//	protected:
//	class HDoc;
		virtual	void	InitInstanceBox(InstanceBox *,ContainerBox *) = 0;	
		virtual	void	FreeInstanceBox(InstanceBox*) = 0;

	protected:
		void	HAlignBox(InstanceBox* box,float maxwidth);
};
#endif
//--
