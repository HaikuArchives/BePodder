#ifndef	_MainModel_
#define	_MainModel_

#include <Entry.h>
#include <Directory.h>
#include <Path.h>

#include "SubscriptionListItem.h"
#include "DownloadListItem.h"
#include "MemoryArchive.h"
#include "GroupItem.h"

#include "KeyMap.h"

class MainModel {
	
	public:
				
				//the full 'list of channels:
				KeyMap<entry_ref,SubscriptionListItem*> fChInfo;
				
				//the archive folder
	    		entry_ref	archive_ref;

	    		//groups
	    		KeyMap<BString,GroupItem*> fGroups;

	    		//current selected ITEM & CHANNEL
	    		MemoryArchive	fCurrentSelectedItem; 		
	    		BNode			fSelectedItem;
	    		BDirectory		fSelectedChannel;	    		
				
				
				// DOWNLOADS:
				
				// entry_ref (of the episode) <--> Object
				KeyMap<entry_ref,DownloadListItem*>		fDownloadList;
					    		
	    		void addDownload(entry_ref ref,DownloadListItem* obj)
	    		{
		    		fDownloadList.AddItem(ref,obj);
	    		}
	    		
	    		DownloadListItem* findDownload(entry_ref ref)
	    		{
	    			bool found=false;
	    			
	    			DownloadListItem* obj=fDownloadList.ValueFor(ref,&found);
	    			if(found) 
	    				return obj;
	    			
	    			return NULL;
				}
	    		
	    		void	removeDownload(entry_ref ref)
	    		{
	    			bool found;
	    			fDownloadList.ValueFor(ref,&found);
	    			if(found) 
	    				fDownloadList.RemoveItemFor(ref);
	    		}

				// EXPERIMENTAL
				SubscriptionListItem *findSubscriptionFromEpisode(entry_ref ref)
				{
					BPath path(&ref);
					path.GetParent(&path);
					entry_ref parent;
					get_ref_for_path(path.Path(),&parent);
					return fChInfo.ValueFor(parent);
				}


};

#endif
