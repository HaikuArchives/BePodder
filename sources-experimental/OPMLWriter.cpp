#include "OPMLWriter.h"
#include "Logger.h"
		
OPMLWriter::OPMLWriter(const BString title):
	doc(NULL),
	body(NULL)
{
	doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr opml = xmlNewNode(NULL, BAD_CAST "opml");
	xmlNewProp(opml, BAD_CAST "version", BAD_CAST "1.0");
	xmlDocSetRootElement(doc, opml);
	xmlNodePtr head = xmlNewChild(opml, NULL, BAD_CAST "head", NULL);
	xmlNewChild(head, NULL, BAD_CAST "title", BAD_CAST title.String());
	body = xmlNewChild(opml, NULL, BAD_CAST "body", NULL);
}


OPMLWriter::~OPMLWriter() {
	xmlFreeDoc(doc);
	xmlCleanupParser();
}



void
OPMLWriter::AddChannel(SubscriptionListItem* item)
{
	if (item == NULL || item->GetTitle() == "" || item->GetURL() == "")
		return;

	xmlNodePtr outline = xmlNewChild(body, NULL, BAD_CAST "outline", NULL);
	xmlNewProp(outline, BAD_CAST "text", BAD_CAST item->GetTitle().String());
	xmlNewProp(outline, BAD_CAST "xmlURL", BAD_CAST item->GetURL().String());
	if (item->GetWeblink() != "") 
		xmlNewProp(outline, BAD_CAST "htmlURL",
			BAD_CAST item->GetWeblink().String());
};

void
OPMLWriter::WriteToFile(const char* filename)
{
	xmlSaveFormatFileEnc(filename, doc, "UTF-8",1);
};
