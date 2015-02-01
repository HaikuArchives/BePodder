/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "IActionMenuItem.h"

IActionMenuItem::IActionMenuItem(IAction* action)
:IconMenuItem(action->GetIcon(IAction::SIZE_16),
			  action->GetLabel().String(),
			  "",
			  action->CopyMessage(),
			  false)
{
	//done. :)	
	uint32 mod;
	const char charz = action->Shortcut(&mod);
	SetShortcut(charz,mod);
};


//--
