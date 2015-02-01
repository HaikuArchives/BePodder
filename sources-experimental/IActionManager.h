/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IActionManager_H_
#define _IActionManager_H_

#include "IAction.h"

#include <Handler.h>
#include <Application.h>
#include <malloc.h>

#define PERFORM_ACTION	'peac'

class IActionManager {
	
	public:
			
				IActionManager(int maxSize){
			 		fActions=(IAction**)malloc(maxSize*sizeof(Action*));
			 		memset(fActions,0,maxSize*sizeof(Action*));
				}
				virtual ~IActionManager(){
					free(fActions);
				}
				
				void SetAction(int pos,IAction* action){
					fActions[pos]=action;
				}
				IAction*	GetAction(int pos){
					return fActions[pos];
				}
				
				static BMessage*	MessageForAction(int pos){
							BMessage* msg=new BMessage(PERFORM_ACTION);
							msg->AddInt16("action_position",pos);
							return msg;
				}
				
				static BHandler*		TargetForAction(int /*pos*/){
							return be_app;
				}	
			
				virtual	void	Perform(BMessage* msg) {
					int16 pos;
					if(msg->FindInt16("action_position",&pos)!=B_OK) return;
					
					IAction *action=GetAction(pos);
					if(!action) return;
					
					action->Perform(msg);
				}
				
							
				
				void	FireAction(int pos){
						be_app->PostMessage(MessageForAction(pos),TargetForAction(pos));
				}
	private:
			
			IAction**	fActions;
	
};
#endif
