#include "OPMLParser.h"
#include "curl/curl.h"

#include "Logger.h"


OPMLTree*			
OPMLParser::Parse(BString filename){
	

	OPMLTree* list=NULL;
	
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	xmlDocPtr doc = xmlCtxtReadFile(ctxt, filename.String(), NULL, XML_PARSE_RECOVER);
	xmlXPathContextPtr pathCtxt = xmlXPathNewContext(doc);
	
	//testing code!
	 xmlXPathObjectPtr itemNode = xmlXPathEvalExpression((const xmlChar *)"/opml/head/title", pathCtxt);
	 if (itemNode == NULL) {
	 	xmlFreeParserCtxt(ctxt);
		xmlXPathFreeContext(pathCtxt);
		xmlXPathFreeObject(itemNode);
		return NULL;
	 };
	 //only one channel per rss file!
	 if(itemNode->nodesetval == NULL || itemNode->nodesetval->nodeNr !=1){
	 	xmlFreeParserCtxt(ctxt);
		xmlXPathFreeContext(pathCtxt);
		xmlXPathFreeObject(itemNode);
		//LOG("OPMLParser", liDebug ,"No title in opml file..");
		return NULL;
	 };
	 
	 //LOG("OPMLParser", liDebug ,"OPML title: %s",XML_GET_CONTENT(itemNode->nodesetval->nodeTab[0]->children));
	 const char* title = (const char*)XML_GET_CONTENT(itemNode->nodesetval->nodeTab[0]->children);
	 BString opml_name(title != NULL ? title : "");
	 
	 
	 
	 if (pathCtxt != NULL) {
		xmlXPathObjectPtr itemNode = xmlXPathEvalExpression((const xmlChar *)"/opml/body/outline", pathCtxt);
		if (itemNode == NULL) {
			xmlFreeParserCtxt(ctxt);
			xmlXPathFreeContext(pathCtxt);
			xmlXPathFreeObject(itemNode);
			return NULL; 
		};
		
		
		
		xmlNodeSetPtr items = itemNode->nodesetval;
				
		if (items == NULL || items->nodeNr < 1 ) {
			xmlFreeParserCtxt(ctxt);
			xmlXPathFreeContext(pathCtxt);
			xmlXPathFreeObject(itemNode);
			return NULL;			
		};
		
		
			//the root
			list = new OPMLTree;
			list->description = opml_name;
			list->foglie = new OPMLList(items->nodeNr);
			
			for (int32 i = 0; i < items->nodeNr; i++) {
				ParseOutline(list, items->nodeTab[i]);
			}
			
 		xmlXPathFreeObject(itemNode);
	};
	
	xmlFreeParserCtxt(ctxt);
	xmlXPathFreeContext(pathCtxt);
	
	return list;
}

int			
OPMLParser::ParseOutlineFolder(OPMLTree* root,xmlNodePtr node)
{
	return 0;	
}

int
OPMLParser::ParseOutline(OPMLTree* aroot,xmlNodePtr node){			

	xmlNode *cur_node = NULL;
    cur_node = node;
    {
        
        if (cur_node->type == XML_ELEMENT_NODE) 
        {            
			BString bname((const char*)cur_node->name);
	
        	if(bname.ICompare("outline") == 0){ 
        	
        		OPMLTree* subtree = new OPMLTree;
				aroot->foglie->AddItem(subtree);					
				
				subtree->foglie = NULL;
       	
        		//guardiamo le properties:
        		xmlAttr *attr = cur_node->properties;
        		xmlAttr *cur_attr = NULL;
        		
    			for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    				BString battr((const char*)cur_attr->name);		
					if(battr.ICompare("text") == 0) {
						xmlNodePtr node = cur_attr->children;
						BString imageURL ;
						imageURL << (const char*)node->content;				
						subtree->description.SetTo(imageURL);
					}  
					else
					if(battr.ICompare("xmlUrl") == 0 || battr.ICompare("url") == 0) {
						xmlNodePtr node = cur_attr->children;
						BString imageURL ;
						imageURL << (const char*)node->content;				
						subtree->link.SetTo(imageURL);
					}
					      	
        		} //for attributes
        		

        		
//				printf("--> Name %s\n", subtree->description.String());       		
        		if (cur_node->children) 
        		{
	        		subtree->foglie = new OPMLList(10);
 //       			printf("--> childs!\n");
					
					xmlNode *cur_child = NULL;
					for (cur_child = cur_node->children; cur_child; cur_child = cur_child->next)
        			{
        				ParseOutline(subtree,	cur_child);
 	       			}
//					printf("--> End Name %s -- %ld\n", subtree->description.String(), subtree->foglie->CountItems());        			
        			//	
        		}
       		}	
        }
    }
	return 0;    
}
/*
int
OPMLParser::ParseOutline(OPMLTree* aroot,xmlNodePtr node){

	xmlNode *cur_node = NULL;

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        
        if (cur_node->type == XML_ELEMENT_NODE) 
        {            
       	    printf("-->DEBUG %s\n", (const char*)cur_node->name);   	
        	BString bname((const char*)cur_node->name);        	
        	
        	if(bname.ICompare("outline") == 0){        	
        		if(aroot->foglie == NULL) aroot->foglie = new OPMLList(10);
        		
        		//LOG("OPMLParser", liDebug ,"New CHILD for [%s]",aroot->description.String());
        		
        		OPMLTree* tree = new OPMLTree;
				tree->foglie = NULL;
				aroot->foglie->AddItem(tree);
				
        		//guardiamo le properties:
        		xmlAttr *attr = cur_node->properties;
        		xmlAttr *cur_attr = NULL;
    			
    			for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    				BString battr((const char*)cur_attr->name);		
					if(battr.ICompare("text") == 0) {
						xmlNodePtr node = cur_attr->children;
						BString imageURL ;
						imageURL << (const char*)node->content;				
						tree->description.SetTo(imageURL);
						
						//LOG("OPMLParser", liDebug ,"\ttext: %s",tree->description.String());
					}  
					else
					if(battr.ICompare("xmlUrl") == 0 || battr.ICompare("url") == 0) {
						xmlNodePtr node = cur_attr->children;
						BString imageURL ;
						imageURL << (const char*)node->content;				
						tree->link.SetTo(imageURL);
						
						//LOG("OPMLParser", liDebug ,"\tlink: %s",tree->link.String());
					}
					      	
        		} //for attributes
        	
        		// ok se non ho 'link' proseguo ricorsivamente altrimenti se ho un link
        		// significa che sono ad una foglia..
        		//if(tree->link != "") return 0;
        		
        		
        		if (cur_node->children) 
        			ParseOutline(tree,	cur_node->children);
        			 
	       		
        }	
        	 
      }
	}
	return 0;
}
*/
void		
OPMLParser::PrintToStream(OPMLTree* tree,int tab)
{
	for(int i=0;i<tab;i++)
		printf("\t");
		
	printf(".%s\n",tree->description.String());
	
	if(tree->foglie)
	{
		for(int i=0;i<tree->foglie->CountItems();i++)
			PrintToStream(tree->foglie->ItemAt(i),tab+1);
	}	
}

void		
OPMLParser::DeleteOPMLTree(OPMLTree* tree){
	
	if(tree->foglie)
	{
		for(int i=0;i<tree->foglie->CountItems();i++)
			DeleteOPMLTree(tree->foglie->ItemAt(i));
		
		tree->foglie->MakeEmpty();
		delete tree->foglie;
		
	}
	
	delete tree;
		
}

