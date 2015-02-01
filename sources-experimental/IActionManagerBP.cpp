/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "IActionManagerBP.h"
#include "IABPChannel.h"
#include "IABPItem.h"
#include "IABPDownload.h"
#include "IABPGroup.h"


IActionManagerBP::IActionManagerBP():IActionManager(IACTION_COUNT){
}

void
IActionManagerBP::init(MainController* c,MainWindow* w){
	

		SetAction(IACTION_CHANNEL_ADD_REQUEST,new IABPChannelAddRequest());
		SetAction(IACTION_CHANNEL_REMOVE, new IABPChannelRemove(c,w));	
		SetAction(IACTION_CHANNEL_CHECK, new IABPChannelCheck(c,w));	
		SetAction(IACTION_CHANNEL_WWW, new IABPChannelWWW(c,w));	
		SetAction(IACTION_CHANNEL_ENCLOSURE_FOLDER,new IABPChannelEnclosureFolder(c,w));
		SetAction(IACTION_CHANNEL_CHECK_ALL, new IABPChannelCheckAll(c,w));
		SetAction(IACTION_CHANNEL_SHOW_IMAGE, new IABPChannelShowImage(c,w));	
		
		SetAction(IACTION_ITEM_REMOVE,new IABPItemRemove(c,w));
		SetAction(IACTION_ITEM_STOP,new IABPItemStop(c,w));
		SetAction(IACTION_ITEM_DOWNLOAD,new IABPItemDownload(c,w));
		SetAction(IACTION_ITEM_PLAY,new IABPItemPlay(c,w));
		SetAction(IACTION_ITEM_WWW,new IABPItemWWW(c,w));
		
		
		SetAction(IACTION_DOWNLOAD_PLAY,new IABPDownloadPlay(c,w));
		SetAction(IACTION_DOWNLOAD_DOWNLOAD,new IABPDownloadDownload(c,w));
		SetAction(IACTION_DOWNLOAD_STOP,new IABPDownloadStop(c,w));
		SetAction(IACTION_DOWNLOAD_FIND_ITEM,new IABPDownloadFindItem(c,w));
		SetAction(IACTION_DOWNLOAD_REMOVE,new IABPDownloadRemove(c,w));
		
		SetAction(IACTION_GROUP_CHECK,new IABPGroupCheck(c,w));
		SetAction(IACTION_GROUP_ADD,new IABPGroupAdd(c,w));
		SetAction(IACTION_GROUP_RENAME,new IABPGroupRename(c,w));
		SetAction(IACTION_GROUP_REMOVE,new IABPGroupRemove(c,w));
		
		for(int i=0;i<IACTION_COUNT;i++)
			GetAction((pb_actions)i)->SetMessage(IActionManager::MessageForAction(i));
}
IActionManagerBP::~IActionManagerBP(){
					//delete all off them!
}
				

//details..				
void 
IActionManagerBP::SetAction(pb_actions pos,IActionBP* action){
			IActionManager::SetAction(pos,action);
}

IActionBP*
IActionManagerBP::GetAction(pb_actions pos){
			return (IActionBP*)IActionManager::GetAction(pos);
}

IActionBP*	
IActionManagerBP::GetAction(int pos){
		return (IActionBP*)IActionManager::GetAction(pos);
}				
				
