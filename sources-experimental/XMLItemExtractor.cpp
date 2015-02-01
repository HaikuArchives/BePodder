/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/


#include "XMLItemExtractor.h"
#include "curl/curl.h"
#include "FileStatus.h"


int
XMLItemExtractor::Parse(xmlNodePtr node){

	xmlNode *cur_node = NULL;
	BString currentCategory("");

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s %s\n", cur_node->name,cur_node->content);
        	       	
        	BString bname((const char*)cur_node->name);	 
        	
        	 if (cur_node->children && (cur_node->children->type == XML_TEXT_NODE || cur_node->children->type == XML_CDATA_SECTION_NODE) ) {
        		 
        		//text elemnt.
        		if(bname.ICompare("title") == 0){
        			NotifyString(ITEM_TITLE,(const char*)xmlNodeGetContent(cur_node->children));
        		} 
	       		else if(bname.ICompare("guid") == 0){
        			NotifyString(ITEM_GUID,(const char*)cur_node->children->content);
	       		}
	       		else if(bname.ICompare("id") == 0){ //ATOM 0.3 support...
        			NotifyString(ITEM_GUID,(const char*)cur_node->children->content);
	       		}
	       		else if(bname.ICompare("link") == 0){
        			NotifyString(ITEM_LINK,(const char*)cur_node->children->content);
	       		}
	       		else if(bname.ICompare("pubDate") == 0){
	       			time_t when = curl_getdate((const char*)cur_node->children->content,NULL);
	       			if(when < 0){
	       				//? not rfc822 ?? 
	       			}
	       			NotifyNotifier(ITEM_PUBDATE,(void*)&when,sizeof(time_t));
	       		}
	       		else if(bname.ICompare("keywords") == 0){
	       			NotifyString(ITEM_KEYWORDS,(const char*)cur_node->children->content);
	       		}
	       	}
        	
        	
        	  if(bname.ICompare("description") == 0 && cur_node->children != NULL){
	       			NotifyString(ITEM_DESCRIPTION,(const char*)cur_node->children->content);
	       		}
	       	else
	       	  if(bname.ICompare("summary") == 0 && cur_node->children != NULL){
	       			NotifyString(ITEM_SUMMARY,(const char*)cur_node->children->content);
	       		}
	       		
        	
        	 {
        	 	//non text elemnt like only attributes or sub-elemnt(not handled)
        	    if(bname.ICompare("enclosure") == 0){
	       		
	       		// managing the enclosure:
	       		// FIX
	       		NotifyInt32(ITEM_ENCLOSURE_FILE_STATUS,NOT_DOWNLOADED);
	       		xmlAttr *attr = cur_node->properties;
	       		
	       		
	       		
	       		xmlAttr *cur_attr = NULL;
    			for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    
				BString battr((const char*)cur_attr->name);		
				//printf("attr url (%ld): %s\n",i,attr->name );
				
				if(battr.ICompare("url") == 0){
					xmlNodePtr node = cur_attr->children;
					//if(node) printf("value %s \n",node->content);
					
					BString imageURL ; //= GetNodeContents(node);
					imageURL << (const char*)node->content;		
					imageURL.RemoveAll("\n");		
										
					BString leaf(imageURL.String());
					int32 last_pos = leaf.FindLast("/");
					
					leaf.Remove(0,last_pos+1);
					
					//printf("outut name: [%s]\n",leaf.String());
					
					NotifyNotifier(ITEM_ENCLOSURE_URL,(void*)imageURL.String(),imageURL.Length());
					NotifyNotifier(ITEM_ENCLOSURE_LOCALPATH,(void*)leaf.String(),leaf.Length());
					//fs = 0;
					//NotifyNotifier(ITEM_ENCLOSURE_FILE_STATUS,&fs,sizeof(FileStatus));
						
				}
				else if(battr.ICompare("length") == 0){
					xmlNodePtr node = cur_attr->children;
					if(node) {
						//printf("size %s \n",node->content);
						off_t size = atoll((const char*)node->content);
						NotifyNotifier(ITEM_ENCLOSURE_LENGTH,(void*)&size,sizeof(off_t));
					}
				
				}
				else if(battr.ICompare("type") == 0){
					xmlNodePtr node = cur_attr->children;
					if(node) {
						//printf("type %s \n",node->content);
						NotifyString(ITEM_ENCLOSURE_TYPE,(const char*)node->content);
					}
				}
				
	       		}
	       		} 
	       		else
	       		if(bname.ICompare("category") == 0){
	       			BString cat =XMLItemExtractor::_parseCategory(cur_node);
					if(cat != "")
						currentCategory << " " << cat;         	 	
				}
				
	       	 	continue;
        	 }	
        }
	}

	if(currentCategory != "")
			NotifyNotifier(CHANNEL_CATEGORY,(void*)currentCategory.String(),currentCategory.Length());       

	return 0;
}



BString						
XMLItemExtractor::_parseCategory(xmlNode* cur_node){

				/*
				<itunes:category text="Technology">
                	<itunes:category text="Information Technology"/>
            	</itunes:category>
            	*/
            	
            	//debugger("category!");
            	
				BString categoryString;
				
				categoryString = _extractCategoryText(cur_node); //TOP LEVEL CATEGORY
				
				if(categoryString == "") return NULL; //safety (should never happen)
				
					
            	       		
	       		xmlNode *sub_node = NULL;
	       		if(!cur_node->children) return categoryString;
	       		
	       		for (sub_node = cur_node->children; sub_node; sub_node = sub_node->next) {
	       			BString sub_name((const char*)sub_node->name);	 
	       			if(sub_name.ICompare("category") ==  0 ){
	       					BString sub_category = _extractCategoryText(sub_node);
	       					if(sub_category != ""){
	       				 		categoryString << " ";
	       				 		categoryString << sub_category;
	       					}
	       			}
	       		}
	       		
	       		//add to the archive
	       		return categoryString;
	       		//NotifyNotifier(CHANNEL_CATEGORY,(void*)categoryString.String(),categoryString.Length());
}
	

BString					
XMLItemExtractor::_extractCategoryText(xmlNodePtr cur_node){
			
				xmlAttr *attr = cur_node->properties;
	       		       		
	       		xmlAttr *cur_attr = NULL;
    			for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    
						BString battr((const char*)cur_attr->name);		
								
						if(battr.ICompare("text") == 0)
						{
							xmlNodePtr node = cur_attr->children;
												
							BString category ;
							category << (const char*)node->content;				
											
							return category;
												
						}
	       		} //end of for.
	       		
	       		return NULL;
}

