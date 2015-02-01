#ifndef QueueFileDownload_H_
#define QueueFileDownload_H_

#include "QueueMultiActions.h"
#include <Looper.h>


class QueueFileDownload : public QueueMultiActions { 

public:
			
			QueueFileDownload(const char* name,int howMany,BLooper* target,uint32 msg):QueueMultiActions(name,howMany){
				fMsg=msg;
				fLooper=target;
			};
			
			
	protected:
				void	ActionPerformed(Action*,status_t,BMessage*);
				void	ActionReadyToPerform(Action*);
				void	SuppressAction(Action*);

	private:
				BLooper*	fLooper;
				uint32		fMsg;
};
#endif
