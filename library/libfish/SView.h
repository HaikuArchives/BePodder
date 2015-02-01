#ifndef SView_H_
#define SView_H_

#include <View.h>
#include <ObjectList.h>
#include "KeyMap.h"
#include "HDoc.h"

#include "BoxRenderer.h"
#include "CompositeRenderer.h"

class SView : public BView
{
	public:
				  	SView(BRect);
			 void 	Draw(BRect r);
			 
			 void	DisplayDocument(HDoc* doc);
			 HDoc*	CurrentDisplayed(){ return fDoc;}
			 
			 void	RegisterBoxRenderer(int id,BoxRenderer*);
			 void	UnregisterBoxRenderer(int id);
			 
			 BoxRenderer*	GetRenderer(int);
			 CompositeRenderer*	GetCompositeRenderer(){ return fCompositeRenderer;};
			 				
			 virtual void GetPreferredSize(float*,float*);
			 
			 void AttachedToWindow();
			 
	private:
			HDoc*			fDoc;
			CompositeRenderer*	fCompositeRenderer;
			
			KeyMap<int,BoxRenderer*> fRenderers;
			bool	fMouseDown;
};

#endif
//--
