/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  DownloadManager_H_
#define DownloadManager_H_

#include "ActionDownload.h"
#include "QueueFileDownload.h"
#include <Looper.h>
#include <List.h>
#include "curl/curl.h"

#define		DOWNLOAD_CHANNEL_INFO	'addc'
#define		DOWNLOAD_ITEM_INFO		'adde'
#define		DOWNLOAD_EXTRA_INFO		'extr'
class DownloadManager {

	public:
			DownloadManager(BLooper* target);

	enum QueueType {
			CHANNELS_QUEUE =0,
			ITEMS_QUEUE,
			EXTRA_QUEUE
	};
	
	void Enqueue(QueueType,ActionDownload*);

	void	TryStopCurrentAction(QueueType,BString key,BString value);
	
	bool	RemoveFromQueue(QueueType,BString key,BString value);
	
	void 	RemoveQueue(QueueType,BList* removed);

	void	LoadProxySetting(BMessage* data);
	void	LoadDownloadSetting(BMessage* data);
	
	

	
	thread_id	SingleThreadAction(ActionAsync* action,bool autoResume=true);
	
	void	FinishCurl(CURL*	curl);

private:

		static int32	SingleThreadPerform(void* a);
	
		QueueFileDownload	*	queue[3];
		
		//proxy
		int32 port;
		BString address;
		BString	userpwd;
		bool	enabled;
		
};

#endif
