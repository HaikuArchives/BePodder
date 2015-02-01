#include "HelpViewer.h"
#include "OneSView.h"
#include <ScrollView.h>
#include <Roster.h>
#include "RVActionGoto.h"
#include "RVActionOpenURL.h"


OneSView	*sview; //temp fix!
BScrollView *scroll; //temp fix

RVActionGoto* agoto;
RVActionOpenURL* aopenurl;

HelpViewer::HelpViewer(BRect r):BView(r,"HelpViewer",B_FOLLOW_ALL_SIDES,B_WILL_DRAW){
	//the view:
	BRect rect(Bounds());
	rect.right -=B_V_SCROLL_BAR_WIDTH;
	//OneSView	*
	
	///BView *avv=new BView(rect,"",B_FOLLOW_ALL,B_WILL_DRAW);
	sview = new OneSView(rect);
	//avv->AddChild(sview);
	scroll= new BScrollView("",sview,B_FOLLOW_NONE,B_WILL_DRAW,false,true,B_NO_BORDER);
	
	rgb_color white = {255,255,255,255};
	rgb_color black = {0,0,0,255};
	backColor = white;
	foreColor = black;
			
	Selected = -1;
	SetViewColor(backColor); 
	AddChild(scroll);
	
	
	sview->AddAction("goto",agoto=new RVActionGoto(this));
	sview->AddAction("openUrl",aopenurl=new RVActionOpenURL(this));
	fGotoHand = NULL;
	
}

void	
HelpViewer::SetForeColor(rgb_color c){ foreColor=c;} ;

void	
HelpViewer::SetBackColor(rgb_color c){ backColor=c; SetViewColor(backColor); scroll->SetViewColor(backColor); sview->SetViewColor(backColor);} ;

HelpViewer::~HelpViewer(){
	//first free all the documents ans InstanceBoxes..
	sview->DisplayDocument(NULL);
	for(int i=0;i<fPages.CountItems();i++)
		delete fPages.ItemAt(i);
	
	fPages.MakeEmpty();

	delete agoto;
	delete aopenurl;
}

void
HelpViewer::SetPage(uint newindex){
	
	if((uint)Selected == newindex) return;
	if(newindex >= fPages.CountItems()) return;
	
	Selected = newindex;
	sview->DisplayDocument(fPages.ItemAt(Selected));
	RecalcScrollbar();
		 
}

int
HelpViewer::CountPage(){
	return fPages.CountItems();
}


void 
HelpViewer::LoadFile(const char* filename){
	
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	xmlDocPtr doc = xmlCtxtReadFile(ctxt, filename, NULL, XML_PARSE_RECOVER);
	xmlXPathContextPtr pathCtxt = xmlXPathNewContext(doc);
	
	{
	  xmlXPathObjectPtr itemNode = xmlXPathEvalExpression((const xmlChar *)"/help/page", pathCtxt);
	  if (itemNode == NULL) {
	 	 	xmlFreeParserCtxt(ctxt);
		 	xmlXPathFreeContext(pathCtxt);
		 	xmlXPathFreeObject(itemNode);
		 	return;
	  };
	  
	  if(itemNode->nodesetval == NULL){
	  	 	xmlFreeParserCtxt(ctxt);
	 		xmlXPathFreeContext(pathCtxt);
	 		xmlXPathFreeObject(itemNode);
	 		return;
	  };
	  
	  printf("Number of pages %d\n",itemNode->nodesetval->nodeNr);
	  for(int i=0;i<itemNode->nodesetval->nodeNr;i++){
	  	ParsePage(itemNode->nodesetval->nodeTab[i]);
	  }
	 
	}
	
	
	xmlFreeParserCtxt(ctxt);
	xmlXPathFreeContext(pathCtxt);
}

void	
HelpViewer::AttachedToWindow(){
	SetBackColor(backColor); 
}


void
HelpViewer::ParsePage(xmlNodePtr node){

	BMessage attrs;
	findAttribute(node,&attrs);
	
	BString id=attrs.FindString("id");
	if(id==NULL || id == "") return;

	HDoc* doc=sview->createHDoc(node->children,&attrs);
	
	if(doc) {
        doc->foreColor=foreColor;
        doc->backColor=backColor;
        fPages.AddItem(doc);
    }	
}

void	
HelpViewer::findAttribute(xmlNodePtr cur_node,BMessage *attrs){
	
	xmlAttr *attr = cur_node->properties;
       		       		
	xmlAttr *cur_attr = NULL;
    for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
        	BString battr((const char*)cur_attr->name);		
			xmlNodePtr node = cur_attr->children;
			attrs->AddString(battr.String(),(const char*)node->content);	
	 } 
	
}


void
HelpViewer::RecalcScrollbar(){
	
	//debugger("recalc");
	HDoc* doc= sview->CurrentDisplayed();
	if(!doc) return;
	
	BScrollBar *bar;
	if (scroll == NULL
  || (bar = scroll->ScrollBar (B_VERTICAL)) == NULL)
    return ;
   
    BRect bounds (Bounds()); 
    float div=bounds.Height() / sview->Frame().Height();
    if( div < 1.0){
     bar->Show();
     bar->SetProportion (div); //bounds.Height() / sview->Frame().bottom);
     bar->SetRange (0.0, sview->Frame().bottom - bounds.Height());
    }
    else {
    
     bar->Hide();
     bar->SetProportion (1); //bounds.Height() / sview->Frame().bottom);
     bar->SetRange (0, 0);
   }
   sview->ScrollTo(0,0);
   scroll->Invalidate();
}

void
HelpViewer::SearchSetPage(BString id){
	
	for(uint i=0;i<fPages.CountItems();i++){
		if(fPages.ItemAt(i)->DocId().Compare(id) == 0){
			SetPage(i);
			if(fGotoHand){
				BMessage*	msg=new BMessage(HELPVIEWER_GOTO);
				msg->AddInt32("selected",i);
				BMessenger(fGotoHand).SendMessage(msg,fGotoHand);
				return;
			}
		}
	}
}

void	
HelpViewer::SetOpenUrlHandler(BHandler* hand){
	aopenurl->SetHandler(hand);
}


void	
HelpViewer::SetGotoHandler(BHandler* hand){
	fGotoHand=hand;
}

void
HelpViewer::MessageReceived(BMessage* msg){

	switch(msg->what){
		case HELPVIEWER_GOTO:
		{
			BString param;
			if(msg->FindString("goto",&param)==B_OK){
					SearchSetPage(param);
				
			}
		}
		break;
		case HELPVIEWER_OPEN_URL:
		{
			BString param;
			if(msg->FindString("url",&param)==B_OK){
				
				BString app("text/html");
				//Net+ app.SetTo("application/x-vnd.Be-NPOS");
				const char *args[] = { param.String(), 0 };
   				be_roster->Launch (app.String(), 1, const_cast<char **>(args));
		 		
			}
		}
		break;
		default:
			BView::MessageReceived(msg);
		break;
	};
}
//..
