#ifndef ImageRenderer_H_
#define ImageRenderer_H_

#include <View.h>
#include "BoxRenderer.h"

class ContainerBox;

class ImageRenderer : public BoxRenderer {

	public:
	
	void			DrawBox(BView* parent,InstanceBox* ibox);
	void	InitInstanceBox(InstanceBox *,ContainerBox *);	
	void	FreeInstanceBox(InstanceBox*);	
	
};
#endif
//--
