/*
 *   Copyright (C) 2007 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  BPEpisode_H_
#define BPEpisode_H_

#include <Node.h>

#include "Extractor.h"
#include "FileStatus.h"
#include "Notifier.h"

class BPEpisode : public BNode , public Notifier { 

	public:
						BPEpisode(entry_ref*);
  						
  						FileStatus			GetStatus();
  						void					SetStatus(FileStatus); //NOTIFY!
  						
  						BString				EnclosureLocalPath();
  						BString				EnclosureLink();
  						BString				Guid();
  						BString				EnclosureType();
  						
};
#endif
