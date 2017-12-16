#include "ActionDownload.h"
#include <File.h>
#include <Path.h>
#include <Mime.h>
#include <NodeInfo.h>
//#include "Logger.h"

#include "DownloadManager.h"
extern DownloadManager*	download_manager;


ActionDownload::ActionDownload(BString URL, entry_ref dest,
												bool allowResume,
												BString fileType,
												BLooper *target,
												uint32 msg
												):ActionAsync(target,msg),

	fDest(dest),
	fAllowResume(allowResume),
	fFileType(fileType)
{
	//try to find username and password
	
	/*int32 dobleslash=URL.FindFirst("//");
	int32 doblepoint=URL.FindFirst(":",dobleslash);
	int32 at=URL.FindFirst("@");
	
	if(at>0){
		
		if(doblepoint>0) { //with pwd
			URL.CopyInto(fUsername,dobleslash+2,doblepoint-dobleslash-2);
			URL.CopyInto(fPassword,doblepoint+1,at - doblepoint-1);
		}
		else
			URL.CopyInto(fUsername,dobleslash+2,at - dobleslash-2);
		
		URL.Remove(dobleslash+2,at - dobleslash-1);
		printf("// %ld : %ld @ %ld %s\n",dobleslash,doblepoint,at,URL.String());
			
	}
	*/
	fUrl = URL;
		
	file = NULL;
	curl=NULL;
	resuming = 0.0;
	SetShouldStop(false);
//	LOG("ActionDownload",liDebug,"downloading URL=%s, to %s",URL.String(),BPath(&dest).Path());
}


status_t
ActionDownload::openFile(BMessage* errors){
	
	uint32 openMode = 0;
	
	if(fAllowResume)
		openMode = B_WRITE_ONLY | B_CREATE_FILE | B_OPEN_AT_END;
	else
		openMode = B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE;
		
	if(file) return B_ERROR;
	
	file = new BFile(&fDest,openMode);
	
	if(!file || file->InitCheck()!=B_OK)
	{
		BString err("Can't open file!");
		errors->AddInt32("status",ERROR_OPENFILE);
		errors->AddString("error",err);
		return B_ERROR;
	}
	

     		
    
    return B_OK;
}


status_t	
ActionDownload::Perform(BMessage* errors){


	//is it these the right place?
	fillMessage(this,errors);
	//
    
    CURLcode res;
    status_t	status = B_OK;

   curl = curl_easy_init();

 	if(!curl) {
 		BString err("Internal error (CURL can't' init)!");
 		errors->AddInt32("status",ERROR_CURL_INIT);
		errors->AddString("error",err);
 		return B_ERROR;
 	}
    
   /* BString fullUrl;
    if(fUsername != "") {
    	
    	if(fPassword == "") fPassword.SetTo("x");
    	
        fUrl.MoveInto(fullUrl,0,fUrl.FindFirst("//")+2);
    	fullUrl << fUsername << ":" << fPassword << "@";
   		fullUrl << fUrl;
    }
    else
    	fullUrl=fUrl;
    
    printf("fullUrl %s\n",fullUrl.String());
    */
    	
    curl_easy_setopt(curl, CURLOPT_URL, fUrl.String());
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this );
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS,FALSE);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,save_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA,this);
	
	curl_easy_setopt(curl, CURLOPT_PASSWDFUNCTION,passwd);
	curl_easy_setopt(curl, CURLOPT_PASSWDDATA,this);	
	
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION , 1); //follow the white link!
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , FALSE); //https
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , FALSE); //https
	
	curl_easy_setopt(curl, CURLOPT_FAILONERROR , 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL , 1);

	download_manager->FinishCurl(curl);
	
	//RESUME
	if(fAllowResume){
			BNode stat(&fDest);
  //  		file->Seek(SEEK_END,0);
 //			size_t resume = (size_t)file->Position();
 			off_t size = 0;
 			stat.GetSize(&size);
 			
 			size_t resume = size;
 			
 			
    		if(resume>0){
    			curl_easy_setopt(curl,	CURLOPT_RESUME_FROM, resume);
    			resuming = (double)resume;
    		}
	}
	
   	BMessage connect(*Message());
   //	connect.what = Command();
   	connect.AddInt32("status",OK_CONNECTING);
	//Target()->PostMessage(&connect);
	Invoke(&connect);
	
	
	
   
    res = curl_easy_perform(curl);
	
	if(res !=0)
	{
		BString err("Can't connect!");
		errors->AddInt32("status",ERROR_PERFORMING);
		errors->AddString("error",err);
		errors->AddInt32("curl_error",res);
		errors->AddBool("should_stop",ShouldStop());
		status = B_ERROR;  
	}	
	else
	{
		errors->AddInt32("status",OK_DOWNLOADED);
	}	 
   
   curl_easy_cleanup(curl);
   
   errors->AddString("path",GetLocalPath());
   errors->AddRef("entry",&fDest);
   
     
      
  if(fFileType != "")
  {
  	printf("Setting file type to: %s\n", fFileType.String());
 	BNodeInfo(file).SetType(fFileType.String());
  }
  else 
  {
  	printf("Ready to call update_mime_info(%s, false, true, false);\n",BPath(&fDest).Path());
	update_mime_info(BPath(&fDest).Path(),false,true,false);
  }
   delete file;   
   file = NULL;
   
   if(res == CURLE_BAD_DOWNLOAD_RESUME || res == CURLE_HTTP_RANGE_ERROR) {
   		// sarebbe bello chiedere.. va be per ora ci riprova e basta!
   		// cosa chiede? che alternative avrebbe se no l'utente?
   		fAllowResume = false;
   		errors->MakeEmpty();
   		Perform(errors);
   }
//	else
//	if(res!=0) {
//		debugger("Found");
//		errors->MakeEmpty();
//   		Perform(errors);
 //   }
  
   return status;
}

BString		
ActionDownload::GetDescription(){
	return "HTTP File download";
}

BString		
ActionDownload::GetLocalPath(){
	return BPath(&fDest).Path();
}

void
ActionDownload::fillMessage(ActionDownload* ad,BMessage* msg){
	//fill with common fileds
	int32 what=msg->what; //preserve original what.
	*msg = ad->extraInfo;
	
	msg->AddInt64("sid",ad->GetActionID());
	msg->AddString("url",ad->fUrl);
	
	msg->what=what;
}
	


void	
ActionDownload::sendProgressX(ActionDownload* ad,double max,double current){
	
	
	BMessage *msg=ad->Message();
	msg->MakeEmpty();
	
	fillMessage(ad,msg);
	

	
	msg->AddInt32("status",OK_PROGRESS);
	msg->AddFloat("max_progress",max);
	msg->AddFloat("current_progress",current);
	//perc
	float div = (current / max ) * 100;
	
	msg->AddInt32("percentage_progress",(int32)floor(div));
	
	double speed = 0;
	if(curl_easy_getinfo(ad->curl,CURLINFO_SPEED_DOWNLOAD,&speed) == CURLE_OK)
			msg->AddFloat("download_speed",speed);

	
	//TOTALE:
	if(curl_easy_getinfo(ad->curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&speed) == CURLE_OK)
			msg->AddFloat("total_size",speed);
	
		// ATTUALE: CURLINFO_SIZE_DOWNLOAD
	if(curl_easy_getinfo(ad->curl,CURLINFO_SIZE_DOWNLOAD,&speed) == CURLE_OK)
			msg->AddFloat("actual_size",speed);
			
	char *array = NULL;
	
	
	if(curl_easy_getinfo(ad->curl,CURLINFO_EFFECTIVE_URL,&array) == CURLE_OK)
			msg->AddString("effective_url",BString(array));
	
	//Andrea: this method is no more used, is the caller (of this class) who decided the filetype,
	// if empty we ask the system to try to identify.. see end of Perform
	/*
	if(ad->fFileType == "" && curl_easy_getinfo(ad->curl,CURLINFO_CONTENT_TYPE,&array) == CURLE_OK) {
			ad->fFileType.SetTo(array);
			BNodeInfo(ad->file).SetType(ad->fFileType.String());
			msg->AddString("filetype",BString(array));
	}*/
			
	ad->Invoke();
}

/*
void
ActionDownload::sendError(BLooper* looper,uint32 amsg,Status st,BString descr){
	BMessage msg(amsg);
	msg.AddInt32("status",st);
	msg.AddString("error",descr);
	looper->PostMessage(&msg);
}
*/
//

int 
ActionDownload::callback(void *data,
                                      double dltotal,
                                      double dlnow,
                                      double ,
                                      double )
{
	ActionDownload *ad=((ActionDownload*)data);
	
	//LOG("ActionDownload",liDebug,"callback total %f, now %f", dltotal,dlnow);
		
	if(ad->Target())
		sendProgressX(ad,ad->resuming+dltotal,ad->resuming+dlnow);
	return 0;
}

size_t 
ActionDownload::save_file( void *ptr, size_t size, size_t nmemb, void *data){ 
	ActionDownload *ad=((ActionDownload*)data);
	
	if(!ad->file) {
		BMessage errors; //how to report these errors??
		if(ad->openFile(&errors) != B_OK) return 0;
	}
	BFile* file=ad->file;
	if(ad->ShouldStop()) return 0;
	return file->Write(ptr,size*nmemb);
}

int 		
ActionDownload::passwd  (void *clientp, const char *prompt, char *buffer, int buflen){
	return 0;
}
		

//.
