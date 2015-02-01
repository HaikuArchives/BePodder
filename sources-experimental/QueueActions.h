#ifndef QueueActions_H_
#define QueueActions_H_

#include <Looper.h>

#define QA_ADD_ACTION 'qaaa'

#include "Action.h"
#include <ObjectList.h>

#include <OS.h>

class QueueActions {

public:
			
						QueueActions(const char* name);
			virtual ~QueueActions();
			
			void		AddAction(Action* );
			
			//BEFORE CALLING CurrentAction, you MUST Lock() the Queue!
						Action*	CurrentAction();
						int32		CountActions();
						Action*	ActionAt(int32 pos);
						void	RemoveActionAt(int32 pos);
						
					
					bool	Lock();
					void	Unlock();
					bool	IsLocked();

	protected:

					static	int32	ManageTheQueue(void*);
			
			virtual	void	ActionReadyToPerform(Action*) = 0;
			virtual	void	ActionPerformed(Action*,status_t,BMessage*) = 0;
			

			
	private:

					

					void		SetCurrentAction(Action*);
							
		thread_id 					fID;
		sem_id						fLock;
		BObjectList<Action>	fList;
		Action*						fCurrentAction;
		bool							fLocked;
};
#endif
