/*
	Licensed under the MIT license.
	Made for Haiku.
*/
#ifndef OPMLWriter_H_
#define OPMLWriter_H_

#include "SubscriptionListItem.h"
#include <File.h>
#include <ObjectList.h>
#include <String.h>

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/parserInternals.h"
#include "libxml/debugXML.h"

class OPMLWriter
{
	public:
   			OPMLWriter(const BString title);
   			virtual 	~OPMLWriter();

   			void	AddChannel(SubscriptionListItem* item);
   			void    WriteToFile(const char *filename);			
	private:
			xmlDocPtr doc;
			xmlNodePtr body;
};
#endif
