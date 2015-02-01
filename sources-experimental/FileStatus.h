#ifndef FileStatus_H_
#define FileStatus_H_

enum FileStatus {
	// IMPORTANTE
	// aggiungere nuovi stati in fondo a meno che non
	// sapete esattamente che pasticcio riskiate di fare!

	ERROR = -1,
	NOT_DOWNLOADED,	// persistent.
	ENQUEQUED,
	DOWNLOADING,
	DOWNLOADED,		//persistent.
	STOPPED,				//should be persistent.
	NOT_FOUND,
	CANT_CONNECT,
	CONNECTING,			//no really a status..
	NEW,						//I hate this! :) *8*
	BAD_FORMAT,			//if channel is not a valid rss file.
	NO_ENCLOSURE	//simple *10*
	
};
#endif
