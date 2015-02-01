/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef XMLChannelExtractor_H_
#define XMLChannelExtractor_H_

#include "Extractor.h"
#include "XMLItemExtractor.h"


class MemoryArchive;

class XMLChannelExtractor : public Extractor
{
		
	public:
   			BList*	Parse(BString filename);

	private:
				XMLItemExtractor	fItemExtractor;   
				int							ParseChannel(xmlNodePtr node,BString leaf);
				void						_parseImage(xmlNodePtr node);
};
#endif
