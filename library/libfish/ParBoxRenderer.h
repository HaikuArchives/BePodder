#ifndef ParBoxRenderer_H_
#define ParBoxRenderer_H_

#include "BoxRenderer.h"
#include <List.h>
struct multi_line {
	int		count;
	BList	line;
};


class ParBoxRenderer : public BoxRenderer {

	public:
	
	InstanceBox*	CreateInstanceBox(BRect preferredSize,BString txt);
	void			DrawBox(BView* parent,InstanceBox* ibox);
	void			InitInstanceBox(InstanceBox* box,ContainerBox*);
	void			FreeInstanceBox(InstanceBox* box);
	
	protected:
		void 	DrawString(const char* string, BView* parent, BRect rect,alignment ali);
		float	StringSize(BFont*,const char* string);
		float	StringSize(BFont* f,const char* string,int lenght);
				
};
#endif
//--
