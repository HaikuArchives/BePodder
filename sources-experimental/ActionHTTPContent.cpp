#include "ActionHTTPContent.h"
#include <Path.h>
#include <Mime.h>
#include <NodeInfo.h>


#include "DownloadManager.h"
extern DownloadManager*	download_manager;



ActionHTTPContent::ActionHTTPContent(BString url){
	curl = NULL;
	SetShouldStop(false);
	SetUrl(url);	
}

ActionHTTPContent::~ActionHTTPContent(){
};

status_t	
ActionHTTPContent::Perform(BMessage* errors){


    
    CURLcode res;
    status_t	status = B_OK;

   curl = curl_easy_init();

 	if(!curl) {
 		BString err("Internal error (CURL can't' init)!");
 		return B_ERROR;
 	}
    
    curl_easy_setopt(curl, CURLOPT_URL, fUrl.String());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,save_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA,this);
	
	
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION , 1); //follow the white link!
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , FALSE); //https
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , FALSE); //https
	
	curl_easy_setopt(curl, CURLOPT_FAILONERROR , 1);
	
	download_manager->FinishCurl(curl);
	
	

    res = curl_easy_perform(curl);
	
	if(res ==0)
  	 AfterPerforming(errors);   

   curl_easy_cleanup(curl);
   return status;
}

BString		
ActionHTTPContent::GetDescription(){
	return "HTTP Content";
}


size_t 
ActionHTTPContent::save_file( void *ptr, size_t size, size_t nmemb, void *data){ 
	ActionHTTPContent *ad=((ActionHTTPContent*)data);
	
	
	if(ad->ShouldStop()) return 0;
	
	ad->content.Append((const char*)ptr, size*nmemb);

	return size*nmemb;
}

		


void
ActionHTTPContent::AfterPerforming(BMessage* errors){
	if(content != "")
		errors->AddString("content",content);
}

//.
