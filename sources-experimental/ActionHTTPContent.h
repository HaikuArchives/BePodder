#ifndef _ActionHTTPContent_h_
#define _ActionHTTPContent_h_

#include "ActionAsync.h"
#include <Entry.h>
#include <Looper.h>

#include <SupportDefs.h>
#include "curl/curl.h"


class ActionHTTPContent : public ActionAsync
{
	public:
				ActionHTTPContent(BString url);
				~ActionHTTPContent();
		
				status_t		Perform(BMessage* errors); 
		 		BString			GetDescription();

				void	SetShouldStop(bool stop){ fShouldStop=stop;}
				bool	ShouldStop(){ return fShouldStop; }
				
				virtual	void	AfterPerforming(BMessage* errors) ;
				
				void	SetUrl(BString url) { fUrl =url; };
				
		
private:
		
		
		BString content;

		//curl callbacks:
		static 	size_t		save_file ( void *ptr, size_t size, size_t nmemb, void *stream);

		CURL*		curl;
		BString		fUrl;		//from.
		bool		fShouldStop;
};

#endif
