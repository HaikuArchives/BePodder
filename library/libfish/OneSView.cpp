#include "OneSView.h"
#include "ParBoxRenderer.h"
#include "TitleBoxRenderer.h"
#include "TextInstanceBox.h"
#include "ImageRenderer.h"
#include "HorizontalContainerBox.h"
#include "ContainerBox.h"
#include "Cursor.h"
#include "RVAction.h"
#ifdef ZETA
	#include <Locale.h>
#else
	#define _T(A) A
#endif
#include <Debug.h>
#define	PAR_SPACE	14

enum{
	TITLE,
	TEXT,
	IMAGE,
	VERTICAL,
	HORIZONTAL
};

// cursor data for hovering over URLs

static unsigned char URLCursorData[] = {16,1,2,2,
  0,0,0,0,56,0,36,0,36,0,19,224,18,92,9,42,
  8,1,60,33,76,49,66,121,48,125,12,253,2,0,1,0,
  0,0,0,0,56,0,60,0,60,0,31,224,31,252,15,254,
  15,255,63,255,127,255,127,255,63,255,15,255,3,254,1,248
};

BCursor *fURLCursor;

//test 
ContainerBox *cbox;

int textSize;  //temp
int titleSize; //temp;

OneSView::OneSView(BRect r):SView(r){
 RegisterBoxRenderer(TITLE,new TitleBoxRenderer());
 RegisterBoxRenderer(TEXT,new ParBoxRenderer());
 RegisterBoxRenderer(IMAGE,new  ImageRenderer());
 cbox = (ContainerBox*)GetCompositeRenderer()->CreateInstanceBox(Bounds(),"null");
  fURLCursor = new BCursor (URLCursorData);
}

OneSView::~OneSView(){
 
 //then free all the renderers..
 delete GetRenderer(TITLE);
 UnregisterBoxRenderer(TITLE);
 
 delete GetRenderer(TEXT);
 UnregisterBoxRenderer(TEXT);
 
 delete GetRenderer(IMAGE);
 UnregisterBoxRenderer(IMAGE);
 
 delete fURLCursor;
}

HDoc*	
OneSView::createHDoc(xmlNodePtr node,BMessage* attrs){
	
	BString id;
	attrs->FindString("id",&id);
	HDoc *doc = new HDoc(id,0,0,10,PAR_SPACE);
    
    checkHDocAttributes(doc,attrs);
       textSize  = doc->textFontSize;
    	titleSize = doc->titleFontSize;
    
    checkContainerAttributes(doc,attrs);
    
    ParsePars(node,doc); 
	 doc->renderer = GetCompositeRenderer();
	 doc->renderer->InitInstanceBox(doc,cbox);
    return doc;
}


void	
OneSView::ParsePars(xmlNodePtr node,ContainerBox* container){
		
		xmlNode *cur_node = NULL;
		BRect rect(0,0,0,0);

    for (cur_node = node; cur_node; cur_node = cur_node->next) {

        if (cur_node->type == XML_ELEMENT_NODE) {
            
             	       	
        	BString bname((const char*)cur_node->name);	 
        	
        	 if (cur_node->children && (cur_node->children->type == XML_TEXT_NODE || cur_node->children->type == XML_CDATA_SECTION_NODE) ) {
        		 
        		//text element..
        		if(bname.ICompare("paragraph") == 0){
        			//
        			BString content((const char*)cur_node->children->content);
        			content.SetTo(_T(content.String()));
        			
        			// type.
        			BMessage attrs;
        			int type = findAttribute(cur_node,&attrs);
        			if(type<0) break;	
        			
        			switch(type){
        				case TITLE:
        				  AddTitle(content.String(),rect,container,&attrs);
        				break;
        				case TEXT:
        				  AddPar(content.String(),rect,container,&attrs);
        				break;
        				case IMAGE:
        				  AddImage(content.String(),rect,container,&attrs);
        				break;
        				default:
        				break;
        			}
        		} 
        		else
        		if(bname.ICompare("block") == 0){
        			
        			//parse degli attributi:
        			BMessage attrs;
        			int type = findAttribute(cur_node,&attrs);
        			ContainerBox* ncontainer = NULL;
        			//creiamo il tipo di block (v o h)
        			switch(type){
        				case VERTICAL:
        					ncontainer = new VerticalContainerBox(rect,"testContainerV",GetCompositeRenderer(),5);
	 						ncontainer->renderer = GetCompositeRenderer();
        				break;
        				case HORIZONTAL:
        					ncontainer = new HorizontalContainerBox(rect,"testContainerH",GetCompositeRenderer(),5);
	 						ncontainer->renderer = GetCompositeRenderer();
        				break;
        				default:
        				
        				break;
        			}
        			//ricorrere è divino:
        			if(ncontainer) {
        				checkContainerAttributes(ncontainer,&attrs);
        				ParsePars(cur_node->children,ncontainer);
        				container->AddInstanceBox(ncontainer);
        			}
        		} 
        		
        		
	       		
	       	}
        	
         }	
        	 
     }					
}
		
void	
OneSView::AddTitle(const char* title,BRect rect,ContainerBox* doc,BMessage* attrs){
	TextInstanceBox* tib=(TextInstanceBox*)GetRenderer(TITLE)->CreateInstanceBox(rect,title);
	tib->text_font.SetSize(titleSize);
	doc->AddInstanceBox(tib);
	checkCommonAttributes(tib,doc,attrs);
	checkTextAttributes(tib,doc,attrs);
}

void	
OneSView::AddPar(const char* title,BRect rect,ContainerBox* doc,BMessage* attrs){
	TextInstanceBox* tib=(TextInstanceBox*)GetRenderer(TEXT)->CreateInstanceBox(rect,title);
	tib->text_font.SetSize(textSize);
	doc->AddInstanceBox(tib);
	checkCommonAttributes(tib,doc,attrs);
	checkTextAttributes(tib,doc,attrs);
}

void	
OneSView::AddImage(const char* title,BRect rect,ContainerBox* doc,BMessage* attrs){
	InstanceBox* tib=GetRenderer(IMAGE)->CreateInstanceBox(rect,title);
	doc->AddInstanceBox(tib);
	checkCommonAttributes(tib,doc,attrs);
	
}

void	
OneSView::checkCommonAttributes(InstanceBox* box,ContainerBox* doc,BMessage* attrs){
	
	BString align;
	if(attrs->FindString("align",&align) == B_OK){
		
		if(align.ICompare("center") == 0){
			box->box_alignment = B_ALIGN_CENTER;
		}
		else if(align.ICompare("right") == 0){
			box->box_alignment = B_ALIGN_RIGHT;
		}
	
	}
	
	BString size;
	if(attrs->FindString("hlimit",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>0)
			box->hlimit = ssize;	
	}
	BString action;
	if(attrs->FindString("action",&action) == B_OK){
		box->action=action;
	}

}
void	
OneSView::checkTextAttributes(TextInstanceBox* box,ContainerBox* doc,BMessage* attrs){
	
	BString size;
	if(attrs->FindString("fontsize",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>0)
			box->text_font.SetSize(ssize);	
	}
	
	
}

void 	
OneSView::checkHDocAttributes(HDoc* doc, BMessage* attrs){
	BString size;
	if(attrs->FindString("textfontsize",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>0)
			doc->textFontSize = ssize;	
	}
	if(attrs->FindString("titlefontsize",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>0)
			doc->titleFontSize = ssize;	
	}

}

void 	
OneSView::checkContainerAttributes(ContainerBox* box, BMessage* attrs){
	BString size;
	if(attrs->FindString("leftmargin",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>=0)
			box->left_margin = ssize;	
	}
	if(attrs->FindString("topmargin",&size) == B_OK){
		int32 ssize=atoi(size.String());
		if(ssize>=0)
			box->top_margin = ssize;	
	}

}




int	
OneSView::findAttribute(xmlNodePtr cur_node,BMessage *attrs){
	
	xmlAttr *attr = cur_node->properties;
	int type = -1;
	       		       		
	xmlAttr *cur_attr = NULL;
    for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    
    	BString battr((const char*)cur_attr->name);		
	
		if(battr.ICompare("type") == 0){
			
			xmlNodePtr node = cur_attr->children;
			BString stype ;
			stype << (const char*)node->content;				
										
			if(stype.ICompare("text") == 0){
				type=TEXT;
			}
			else
			if(stype.ICompare("title") == 0){
				type=TITLE;
			}
			else
			if(stype.ICompare("image") == 0){
				type= IMAGE; 
			}
			else
			if(stype.ICompare("vertical") == 0){
				type= VERTICAL; 
			}
			else
			if(stype.ICompare("horizontal") == 0){
				type= HORIZONTAL; 
			}
				
		}
		else 
		{	
			xmlNodePtr node = cur_attr->children;
			if(node){
				battr.ToLower();
				attrs->AddString(battr.String(),(const char*)node->content);	
			}
		}
	 } 
	 
	 //attrs->PrintToStream();
	 return type;
}



void
OneSView::MouseDown( BPoint /*where*/ )
{
/*	SetMouseEventMask( B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS );
	
	fMouseDown = true;
	
	Invalidate();
	*/
}


void
OneSView::MouseMoved( BPoint p, uint32 transition, const BMessage * )
{
	switch ( transition )
	{
		case B_ENTERED_VIEW:
		case B_INSIDE_VIEW:
			if(CurrentDisplayed())
				if(GetCompositeRenderer()->HasAction(CurrentDisplayed(),p)){
					SetViewCursor (fURLCursor);  //B_CURSOR_I_BEAM 
					//printf("view c\n");
				}
				else
					SetViewCursor (B_CURSOR_SYSTEM_DEFAULT);
		break;
		case B_EXITED_VIEW:
			//set mouse normal
			SetViewCursor (B_CURSOR_SYSTEM_DEFAULT);
		break;
	}
	
}


void
OneSView::MouseUp( BPoint p )
{
	BString action;
	if(CurrentDisplayed())
		if(GetCompositeRenderer()->HasAction(CurrentDisplayed(),p,&action))
			InvokeAction(action.String());
}

void
OneSView::ClearActions(){
	actions.MakeEmpty();
}

void
OneSView::AddAction(const char* symbolicName,RVAction* action){
	actions.AddPointer(symbolicName,(void*)action);
}


RVAction*
OneSView::FindAction(const char* symbolicName){
	RVAction* action=NULL;
	actions.FindPointer(symbolicName,(void**)&action);
	return action;
}

void
OneSView::InvokeAction(BString mangledName){
	
	int from = mangledName.FindFirst("(");
	
	BString actionName;
	actionName.SetTo(mangledName,from);
		
	mangledName.Remove(0,from+1);
	int	to = mangledName.FindFirst(")");
	
	BString param;
	param.SetTo(mangledName,to);
	
	RVAction* action=FindAction(actionName.String());
	if(action)
		action->Perform(param);
	else
		debugger(actionName.String());
}

