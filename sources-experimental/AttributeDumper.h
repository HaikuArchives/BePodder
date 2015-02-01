#ifndef AttributeDumper_H_
#define AttributeDumper_H_

//Dumps ItemProperty as file attribute.

#include "ElementListener.h"

#include <Path.h>
#include <Node.h>
#include <Entry.h>
#include <Bitmap.h>

class AttributeDumper : public ElementListener {

public:
				 		AttributeDumper(entry_ref*	ref);
				 		//AttributeDumper(node_ref*	nref);
				 		
				 		//create the file:
				 		AttributeDumper(entry_ref*,bool asdir);
				 		
				 		
				 		
					  virtual ~AttributeDumper();
				 
			void KeyModified(int32 key, void* data, ssize_t numBytes);
			
			BPath	GetPath(){ return fPath; }
			void	GetNodeRef(node_ref* ref){ if(fNode) fNode->GetNodeRef(ref); }
			void	GetEntryRef(entry_ref* ref){ *ref=fRef; }
private:
			void 	_writeAttr(const char* name,BString data,bool cut = true);
			status_t	_addBitmap(BBitmap* icon);
			
			BNode*  fNode;
			entry_ref fRef;
			BPath	fPath;
};

#endif
