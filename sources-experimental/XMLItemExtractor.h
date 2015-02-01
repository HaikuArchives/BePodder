/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef XMLItemExtractor_H_
#define XMLItemExtractor_H_

#include "Extractor.h"

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"


class XMLItemExtractor : public Extractor
{
		
	public:
   
			int	Parse(xmlNodePtr node);   
			
			
			//general purpose:
			
			static BString					_parseCategory(xmlNodePtr node);
			static BString					_extractCategoryText(xmlNodePtr node);
  		
};
#endif
