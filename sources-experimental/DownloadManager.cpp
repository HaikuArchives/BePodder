#include "DownloadManager.h"
#include <Messenger.h>

#define DEFAULT_CHANNELS_DOWNLOAD	3
#define DEFAULT_ITEMS_DOWNLOAD		1

DownloadManager::DownloadManager(BLooper* target){
	
	queue[CHANNELS_QUEUE] = new QueueFileDownload("qSubscriptions",DEFAULT_CHANNELS_DOWNLOAD,target,DOWNLOAD_CHANNEL_INFO);
	queue[ITEMS_QUEUE] = new QueueFileDownload("qEnclosures",DEFAULT_ITEMS_DOWNLOAD,target,DOWNLOAD_ITEM_INFO);
	queue[EXTRA_QUEUE] = new QueueFileDownload("qExtra",1,target,DOWNLOAD_EXTRA_INFO);
}

void 
DownloadManager::Enqueue(QueueType type,ActionDownload* ad){
	queue[type]->AddAction(ad);
}

void
DownloadManager::TryStopCurrentAction(QueueType type,BString key,BString value){
	
	if(queue[type]->Lock()){
		for(int i=0;i<queue[type]->CountThreads();i++) {
		 ActionDownload *ad = (ActionDownload*)queue[type]->CurrentAction(i);
		 
		 if(ad &&  ad->GetKey(key).Compare(value.String()) ==0 )
			ad->SetShouldStop(true);
		}		
		
		queue[type]->Unlock();
	}
}

bool
DownloadManager::RemoveFromQueue(QueueType type ,BString key,BString value){
	if(queue[type]->Lock()){
		
		for(int32 i=0;i<queue[type]->CountActions();i++){
		 ActionDownload *ad = (ActionDownload*)queue[type]->ActionAt(i);
		 if(ad->GetKey(key).Compare(value.String()) ==0 ){
			queue[type]->RemoveActionAt(i);
		 	queue[type]->Unlock();
		 	return true;
		 }
		}
		 
		for(int i=0;i<queue[type]->CountThreads();i++) {
		 ActionDownload *ad = (ActionDownload*)queue[type]->CurrentAction(i);
		 
		 if(ad &&  ad->GetKey(key).Compare(value.String()) ==0 )
			ad->SetShouldStop(true);
		}		
		queue[type]->Unlock();
		return false;
	}
	return false;	
}

void
DownloadManager::RemoveQueue(QueueType type,BList* removed){
	
	
	queue[type]->Lock();
	
	while(queue[type]->CountActions())
	{
			//ActionDownload *ad = (ActionDownload*)
			queue[type]->ActionAt(0);
			queue[type]->RemoveActionAt(0);
	}
	
	// ***************************************
	//	we did not unlock
	//  so no one can add new item.
	// ***************************************
		
	for(int i=0;i<queue[type]->CountThreads();i++) {
		if(queue[type]->CurrentAction(i))
			removed->AddItem( (void*)queue[type]->CurrentAction(i));
	}		
}

void
DownloadManager::LoadProxySetting(BMessage* data){
	
	bool value;
	
	enabled=false;
	address.SetTo("");
	userpwd.SetTo("");
	port = 0;
		
	
	if(data->FindBool("enable",&value)==B_OK)
		enabled=value;
	
	if(!enabled) return;
	
	BString username,password;		
	data->FindString("username",&username);	
	data->FindInt32("port",&port);
	data->FindString("address",&address);
	data->FindString("password",&password);
	if(username !="" || password !="")
		userpwd << username << ":" << password;	
	
}


void	
DownloadManager::LoadDownloadSetting(BMessage* data){
	
	//here magic stuff! :=)
	int32 number = 1;
	if(data->FindInt32("max_downloads",&number) == B_OK){
		queue[ITEMS_QUEUE]->SetDownloadCount(number);
	}
}

thread_id
DownloadManager::SingleThreadAction(ActionAsync* action,bool autoResume){
	
	thread_id id = spawn_thread(DownloadManager::SingleThreadPerform,"single_action_thread",B_NORMAL_PRIORITY,(void*)action);
	if(autoResume) resume_thread(id);
	return id;
}

int32
DownloadManager::SingleThreadPerform(void* a){
			
			ActionAsync*	ad=(ActionAsync*)a;
			
			// perform the action
			BMessage err;
			
			//status_t status = 
			ad->Perform(&err); 			
			// do post-perform!
			
			if(ad->Target()){
				err.what=ad->Command();
				//BMessenger(ad->Looper()).SendMessage(&err);
				ad->Invoke(&err);
				}
			return 0;
}


void	
DownloadManager::FinishCurl(CURL*	curl){
	if(!enabled) return;
	
	curl_easy_setopt(curl, CURLOPT_PROXY, address.String());
	curl_easy_setopt(curl, CURLOPT_PROXYPORT, port);
	curl_easy_setopt(curl, CURLOPT_PROXYTYPE , CURLPROXY_HTTP);
	//curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC );
	if(userpwd != "") {
		curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD , userpwd.String() );
		//println("auth %s\n",userpwd.String());		
	}
}
