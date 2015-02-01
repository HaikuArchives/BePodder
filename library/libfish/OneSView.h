#ifndef OneSView_H_
#define OneSView_H_

#include "SView.h"
#include "HDoc.h"

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"

#include "Message.h"

class TextInstanceBox;
class RVAction;

class OneSView : public SView {
	public:
		 OneSView(BRect r);
		~OneSView();  
		
	HDoc*	createHDoc(xmlNodePtr node,BMessage* attrs);		
			void	AddAction(const char* symbolicName,RVAction*);	//by xeD
	
	protected:
		void	AddTitle(const char* title,
						 BRect rect,
						 ContainerBox*,
						 BMessage *attrs);
						 
		void	AddPar(const char* title,
					   BRect rect,
					   ContainerBox*,
					   BMessage *attrs);
					   
		void	AddImage(const char* title,
					   BRect rect,
					   ContainerBox*,
					   BMessage *attrs);
	
		void 	MouseDown( BPoint );
		void 	MouseUp( BPoint );
		void	MouseMoved( BPoint , uint32 transition, const BMessage *);
		

						
	private:
			void	ParsePars(xmlNodePtr node,ContainerBox*);
			void	checkCommonAttributes(InstanceBox* box,ContainerBox* doc,BMessage* attrs);
			void	checkTextAttributes(TextInstanceBox* box,ContainerBox* doc,BMessage* attrs);
			void 	checkHDocAttributes(HDoc*, BMessage*);
			void 	checkContainerAttributes(ContainerBox*, BMessage*);
			
			void	InvokeAction(BString mangledName);
			void	ClearActions(); 		//by xeD

			RVAction*				FindAction(const char* symbolicName);	//by xeD
	
			//return type (as int)
			int		findAttribute(xmlNodePtr node,BMessage *attrs);
			
				BMessage			actions; //by xeD
};
#endif
