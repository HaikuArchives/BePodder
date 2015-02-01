#include "XMLChannelExtractor.h"

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"

#include "RssItem.h"

#include "curl/curl.h"

#include "MemoryArchive.h"

//#define	CHANNEL_QUERY	"/rss/channel" 
#define	CHANNEL_QUERY	"//*[local-name()='channel']"

#define	FEED_QUERY	"//*[local-name()='feed']"

//#define	ITEM_QUERY			"/rss/channel/item"
//#define	ITEM_QUERY			"//*[local-name()='item']"
//#define	ENTRY_QUERY			"//*[local-name()='entry']"


#define START_QUERY "//*[local-name()='"
#define END_QUERY	"']"

BList*			
XMLChannelExtractor::Parse(BString filename){
	
	BList* list=NULL;
	
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	xmlDocPtr doc = xmlCtxtReadFile(ctxt, filename.String(), NULL, XML_PARSE_RECOVER|XML_PARSE_NOBLANKS|XML_PARSE_NOCDATA);
	xmlXPathContextPtr pathCtxt = xmlXPathNewContext(doc);
	
	BString elementType("item");

	//testing code!
	{
	 xmlXPathObjectPtr itemNode = xmlXPathEvalExpression((const xmlChar *)CHANNEL_QUERY, pathCtxt);
	 if (itemNode == NULL) {
	 		xmlFreeParserCtxt(ctxt);
			xmlXPathFreeContext(pathCtxt);
			xmlXPathFreeObject(itemNode);
			return NULL;
	 };
	 //only one channel per rss file!
	 if(itemNode->nodesetval == NULL || itemNode->nodesetval->nodeNr !=1){
	 		//we can try to search for a 'feed'
			 xmlXPathFreeObject(itemNode);
			 itemNode = xmlXPathEvalExpression((const xmlChar *)FEED_QUERY, pathCtxt);
			 if (itemNode == NULL) {
	 			xmlFreeParserCtxt(ctxt);
				xmlXPathFreeContext(pathCtxt);
				xmlXPathFreeObject(itemNode);
				return NULL;
	 		};
	 		if(itemNode->nodesetval == NULL || itemNode->nodesetval->nodeNr !=1){
	 	 		xmlFreeParserCtxt(ctxt);
				xmlXPathFreeContext(pathCtxt);
				xmlXPathFreeObject(itemNode);
				return NULL;
	 		}
	 		elementType.SetTo("entry");
		
	 };
	 
	 ParseChannel(itemNode->nodesetval->nodeTab[0]->children,elementType);
	 
	 /*
	 	Testing code.
	 
	 */
	 
	 //parsing the channel's attributes..
	 
	 
	 
	}
	
	BString elementQuery(START_QUERY);
	elementQuery << elementType << END_QUERY;
	
	if (pathCtxt != NULL) {
		xmlXPathObjectPtr itemNode = xmlXPathEvalExpression((const xmlChar *)elementQuery.String(), pathCtxt);
		if (itemNode == NULL) {
			xmlFreeParserCtxt(ctxt);
			xmlXPathFreeContext(pathCtxt);
			xmlXPathFreeObject(itemNode);
			return NULL; 
		};
		
	xmlNodeSetPtr items = itemNode->nodesetval;
		if (items == NULL) {
			xmlFreeParserCtxt(ctxt);
			xmlXPathFreeContext(pathCtxt);
			xmlXPathFreeObject(itemNode);
			return NULL;			
		};
		
			
			
			list=new BList(items->nodeNr);
			
			for (int32 i = 0; i < items->nodeNr; i++) {
			
			MemoryArchive *ma=new MemoryArchive();
			
			RssItem	rss;
			rss.AddElementListener(ma);
			
			fItemExtractor.SetNotifier(&rss);
			fItemExtractor.Parse(items->nodeTab[i]->children);
			
			rss.SetFileStatus(NEW);
			
					
			if( rss.Guid() != "" )
				list->AddItem((void*)ma);
			else
			if( rss.EnclosureLink() != "") {
				rss.SetGuid(rss.EnclosureLink().String());
				list->AddItem((void*)ma);
			}
			else
			if( ma->GetDataString(ITEM_LINK) !=""){
				rss.SetGuid(ma->GetDataString(ITEM_LINK).String());
				list->AddItem((void*)ma);
			}
			else
			if(ma->GetDataString(ITEM_PUBDATE)  !=""){
				rss.SetGuid(ma->GetDataString(ITEM_PUBDATE) .String());
				list->AddItem((void*)ma,0);			
			}
			else 
			{
				rss.RemoveElementListener(ma);
				delete ma; //!
				ma = NULL;
			}
			
			//post process fixes:
			
			if(ma) {
				  if(ma->GetDataString(ITEM_PUBDATE)  =="") {
					//if no data, add the now date!
					time_t when = real_time_clock() - i;			
					ma->SetKey(ITEM_PUBDATE,(void*)&when,(ssize_t)sizeof(time_t));
				  }
				  if(ma->GetDataString(ITEM_LINK) == ma->GetDataString(ITEM_ENCLOSURE_URL)){
				  	ma->SetKeyString(ITEM_LINK,"");
				  }
				  if(ma->GetDataString(ITEM_TITLE) == "" && ma->GetDataString(ITEM_ENCLOSURE_LOCALPATH) != ""){
				  		ma->SetKeyString(ITEM_TITLE,ma->GetDataString(ITEM_ENCLOSURE_LOCALPATH).String());
				  }
			     if(ma->GetDataString(ITEM_DESCRIPTION) == "" && ma->GetDataString(ITEM_SUMMARY) != ""){
					  	ma->SetKeyString(ITEM_DESCRIPTION,ma->GetDataString(ITEM_SUMMARY).String());
				 }
			}
						
			rss.RemoveElementListener(ma);	
		}
 		xmlXPathFreeObject(itemNode);
	};
	
	xmlFreeParserCtxt(ctxt);
	xmlXPathFreeContext(pathCtxt);
	
	return list;
}


int
XMLChannelExtractor::ParseChannel(xmlNodePtr node,BString leaf){

	xmlNode *cur_node = NULL;
	BString currentCategory("");

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            
        //    printf("ParseChannel type: Element, name: %s %s\n", cur_node->name,cur_node->content);
        	       	
        	BString bname((const char*)cur_node->name);	 
        	
        	 if (cur_node->children && (cur_node->children->type == XML_TEXT_NODE || cur_node->children->type == XML_CDATA_SECTION_NODE) ) {
        		 
        		//text element..
        		if(bname.ICompare("title") == 0){
        			BString title((const char*)cur_node->children->content);
        			title.RemoveAll("\n");
        			NotifyString(CHANNEL_TITLE,title.String());
        		} 
	       		else
	       		if(bname.ICompare("link") == 0){
        			NotifyString(CHANNEL_WEBLINK,(const char*)cur_node->children->content);
        		} 
	       		
	       		else if(bname.ICompare("description") == 0 && cur_node->children != NULL){
	       			NotifyString(CHANNEL_DESCRIPTION,(const char*)cur_node->children->content);
	       		}
	       		
	       		/*
	       		else if(bname.ICompare("link") == 0){
        			NotifyString(,(const char*)cur_node->children->content);
	       		}
	       		else if(bname.ICompare("pubDate") == 0){
	       			time_t when = curl_getdate((const char*)cur_node->children->content,NULL);
	       			NotifyNotifier(,(void*)&when,sizeof(time_t));
	       		}
	       		*/
	       	}
        	
        		//non text element like only attributes or sub-elemnt(not handled)
        	 {
        	 	if(bname.ICompare("image") == 0)
        	 	{
        	 		_parseImage(cur_node);	
        	 	}//end - image
        	 	else
        	 	if(bname.ICompare("category") == 0){
        	 		
        	 		BString cat =XMLItemExtractor::_parseCategory(cur_node);
					if(cat != "")
						currentCategory << " " << cat; //NotifyNotifier(CHANNEL_CATEGORY,(void*)cat.String(),cat.Length());        	 	
				}
				else
				if(bname.ICompare(leaf) == 0)
					goto finished;
				
        	 }	
        	 
        }
	}
	
	finished:
		if(currentCategory != "")
			NotifyNotifier(CHANNEL_CATEGORY,(void*)currentCategory.String(),currentCategory.Length());        	 
	return 0;
}

void						
XMLChannelExtractor::_parseImage(xmlNode* cur_node){
	       		
	       		xmlAttr *attr = cur_node->properties;
	       		       		
	       		xmlAttr *cur_attr = NULL;
    			for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    
						BString battr((const char*)cur_attr->name);		
								
						if(battr.ICompare("href") == 0)
						{
							xmlNodePtr node = cur_attr->children;
							//if(node) printf("value %s \n",node->content);
					
							BString imageURL ;
							imageURL << (const char*)node->content;				
											
							NotifyNotifier(CHANNEL_IMAGE_URL,(void*)imageURL.String(),imageURL.Length());
							return;
												
						}
	       		} //end of for.
	       		
	       		      		
	       		xmlNode *sub_node = NULL;
	       		if(!cur_node->children) return;
	       		
	       		for (sub_node = cur_node->children; sub_node; sub_node = sub_node->next) {
	       			BString sub_name((const char*)sub_node->name);	 
	       			if(sub_name.ICompare("url") ==  0 ){
	       				 if (sub_node->children && sub_node->children->type == XML_TEXT_NODE){
	       				 	//save the url!
	       				 	BString imageURL ;
							imageURL << (const char*)sub_node->children->content;											
							NotifyNotifier(CHANNEL_IMAGE_URL,(void*)imageURL.String(),imageURL.Length());
							return;
	       				 }
	       			}
	       		}// end sub_image!
}
	

	
