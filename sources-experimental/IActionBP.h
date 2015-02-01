/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IActionBP_h_
#define _IActionBP_h_

#include "IAction.h"

class MainController;
class MainWindow;

class IActionBP : public IAction
{
	public:
			
			 	  IActionBP(MainController* controller = NULL,MainWindow* view = NULL):IAction(){
			 	  		fController=controller;
			 	  		fView=view;	
			 	  };
		
		virtual	~IActionBP(){};
		
			
		protected:
			MainController* fController;
			MainWindow*	fView;

};

#endif
