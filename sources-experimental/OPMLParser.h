/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef OPMLParser_H_
#define OPMLParser_H_

#include <ObjectList.h>
#include <String.h>

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"

struct OPMLTree;

typedef BObjectList<OPMLTree>	OPMLList;

struct OPMLTree  {
	BString					description;
	BString					link;
	OPMLList				*foglie;
};



class OPMLParser
{
		
	public:
   			OPMLTree*	Parse(BString filename);
   			
   			void		DeleteOPMLTree(OPMLTree*); //recursive delete
			
			void		PrintToStream(OPMLTree*,int tab);
	private:
			int			ParseOutline(OPMLTree* root,xmlNodePtr node);
			int			ParseOutlineFolder(OPMLTree* root,xmlNodePtr node);
  		
};
#endif
