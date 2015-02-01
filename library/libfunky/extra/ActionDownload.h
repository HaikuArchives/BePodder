#ifndef _ActionDownload_h_
#define _ActionDownload_h_

#include "ActionAsync.h"
#include <Entry.h>
#include <Looper.h>

#include "curl/curl.h"

class ActionDownload : public ActionAsync
{
	public:
				ActionDownload(BString URL, entry_ref dest,bool allowResume,BLooper *target = NULL ,uint32 msg = 0);
				~ActionDownload(){};
		
				status_t		Perform(BMessage* errors); 
		 		BString		GetDescription();
		 		BString		GetLocalPath();	

				//void		SetLooper(BLooper *loop,uint32 msg){ fTarget=loop; fMsg=msg;};
				
				//for compaatibility:
				void		SetExtraData(BString extra){ 
								extraInfo.AddString("extra",extra); 
							}
							
				void		SetKey(BString key, BString data){
								extraInfo.AddString(key.String(),data);
							}

				void		SetRef(BString key, entry_ref *ref){
								extraInfo.AddRef(key.String(),ref);
							}
			
				status_t GetRef(BString key, entry_ref* ref){
								return extraInfo.FindRef(key.String(),ref);
							}
							
				BString	GetKey(BString key){
					BString data;
					extraInfo.FindString(key.String(),&data);
					return data;
				}
				
					
				void	SetShouldStop(bool stop){ fShouldStop=stop;}
				bool	ShouldStop(){ return fShouldStop; }
	
	enum Status {
		ERROR_OPENFILE,		// 0
		ERROR_CURL_INIT,		// 1
		ERROR_PERFORMING,	// 2
		OK_CONNECTING,		// 3
		OK_PROGRESS,			// 4
		OK_DOWNLOADED		// 5
	};

private:
		
		
						status_t		openFile(BMessage*);
		
		static 		void		fillMessage(ActionDownload*,BMessage*);
		static		void		sendProgressX(ActionDownload*,double max,double current);
		
		//curl callbacks:
		static 	size_t	save_file ( void *ptr, size_t size, size_t nmemb, void *stream);
		static 	int 		callback (void *clientp,double dltotal,double dlnow,double , double);
		static 	int 		passwd  (void *clientp, const char *prompt, char *buffer, int buflen);


		CURL*		curl;
		BString		fUrl;		//from.
		entry_ref	fDest;
		//BLooper*	fTarget; 	//target progess
		//uint32		fMsg;
		BMessage	extraInfo;	
		bool		fShouldStop;
		BFile*		file;
		double		resuming;
		bool		fAllowResume;
		
		BString	fFileType;
};

#endif
