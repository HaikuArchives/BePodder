/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#ifndef  _IActionManagerBP_H_
#define _IActionManagerBP_H_

#include "IActionBP.h"

#include "IActionManager.h"

enum pb_actions {
						ACTION_CHANNEL,
						IACTION_CHANNEL_ADD_REQUEST = 0,
						IACTION_CHANNEL_REMOVE,
						IACTION_CHANNEL_CHECK,
						IACTION_CHANNEL_WWW,
						IACTION_CHANNEL_ENCLOSURE_FOLDER,
						IACTION_CHANNEL_CHECK_ALL,
						IACTION_CHANNEL_SHOW_IMAGE,
						
						IACTION_ITEM_REMOVE,
						IACTION_ITEM_STOP,
						IACTION_ITEM_DOWNLOAD,
						IACTION_ITEM_PLAY,
						IACTION_ITEM_WWW,
						
						IACTION_DOWNLOAD_PLAY,
						IACTION_DOWNLOAD_DOWNLOAD,
						IACTION_DOWNLOAD_STOP,
						IACTION_DOWNLOAD_FIND_ITEM,
						IACTION_DOWNLOAD_REMOVE,
						
						IACTION_GROUP_CHECK,
						IACTION_GROUP_ADD,
						IACTION_GROUP_RENAME,
						IACTION_GROUP_REMOVE,
						
						//Don't remove this!
						IACTION_COUNT
				};
				

class IActionManagerBP : public IActionManager {
	
	public:
				
			
				IActionManagerBP();
				~IActionManagerBP();
				
				
				void init(MainController*,MainWindow*);
				
				
				
				IActionBP*	GetAction(pb_actions pos);
				
				IActionBP*	GetAction(int pos);
				
	private:
			void SetAction(pb_actions pos,IActionBP* action);			
		
};
#endif
