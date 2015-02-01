#ifndef HelpViewer_H_
#define HelpViewer_H_

#include <View.h>

//#include "KeyMap.h"
#include <ObjectList.h>
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"

class HDoc;

#define HELPVIEWER_OPEN_URL 'opur'
#define HELPVIEWER_GOTO		'goto'

class HelpViewer : public BView {

	public:
		 HelpViewer(BRect r);
		virtual ~HelpViewer();
		
		void 	LoadFile(const char* file);
		void	ParsePage(xmlNodePtr node);
		
		void	SetPage(uint);
		int		CountPage();
		
		void	SetForeColor(rgb_color c);
		void	SetBackColor(rgb_color c);
		
		void	SetOpenUrlHandler(BHandler* hand);
		void	SetGotoHandler(BHandler* hand);
		
		void	MessageReceived(BMessage* msg);
		void	SearchSetPage(BString id);
		
		void	AttachedToWindow();
		
	private:
	
		
		void RecalcScrollbar();
		void findAttribute(xmlNodePtr node,BMessage*);
		
		
		
		//KeyMap<BString,HDoc*>	fPages;
		BObjectList<HDoc>	fPages;
		int16	Selected;
		
		rgb_color	foreColor;
		rgb_color	backColor;
		BHandler	*fGotoHand;
	
};
#endif
// --
