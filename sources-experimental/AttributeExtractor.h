/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef AttributeExtractor_H_
#define AttributeExtractor_H_

#include <Node.h>

#include "Extractor.h"



class AttributeExtractor : public Extractor
{
		
public:
   
			void		Parse(BNode* node, bool deep = false);   
  		
};
#endif
