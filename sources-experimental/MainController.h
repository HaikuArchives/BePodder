#ifndef _MainController_
#define _MainController_

#include "MainModel.h"
#include "Notifier.h"
#include <set>

class MainWindow;
class BMessageRunner;
class EpisodeListView;

class MainController {
	
		public:
					MainController(MainWindow* view, MainModel* model);
					
				//directory parser.
				
				void	ParseArchive();

				void	CreateChannelItem(BEntry*,BMessage*);
		
				void	InfoDownloadItem(BMessage* msg);
			
				void	SetSelectedItem(entry_ref* ref,entry_ref *nref); //** cambiare nome! loadInfoItem(ref) **//

				const MemoryArchive*	GetSelectedItem(); //anche questo è da eliminare.. chi lo usa??
				
				/*** generic ***/
				
				void			WWWItem(entry_ref);
			
				bool			DeleteEpisode(entry_ref); //true if the episode was NEW
				
				status_t		PlayItem(entry_ref); 
				
				status_t		DownloadItem(entry_ref,EpisodeListItem* e=NULL);

				status_t		StopDownloadItem(entry_ref);
				
				status_t		FindItem(entry_ref);
								
				status_t		RemoveDownload(entry_ref);
				
				
				/***** Channels ***********/
				
				
				void			ShowChannelImage(entry_ref);
				void			DownloadChannel(BMessage* msg);
				void			InfoDownloadChannel(BMessage* msg);
				void			FolderSelectedChannel(entry_ref nref);
				
				BString		PathFolderChannel(entry_ref nref);
				void			ArchiveChannel(entry_ref nref);
				status_t		DeleteChannel(entry_ref nref);
				
				void			CheckChannel(entry_ref nref);
				void			CheckAllChannel();
				void			SwitchChannel(entry_ref* nref);
				
				
				//groups
				
				void	AddNewGroup(const char* label);
				void	RenameGroup(BString oldname,BString newname);	
				void	RemoveGroup(BString name);
				
				void 	InfoExtraDownload(BMessage* msg);	
											

				
				void	OpenURL(BString link);
				
				void	OpenFile(const char* filename,BString filetype);
				
				void	QuitRequested();
				
				void	LoadSettings(const char* type,BMessage* data);
					
		private:

					void	SaveGroupSetting(BMessage*);
					void	LoadGroupSetting(BMessage*);
					void	StoreStates();
					void	RealRemoveDownload(DownloadListItem*);
					void	UpdateSelected();
					
					void	UnlinkDownloadItems();
		
					GroupItem*	AddSavedGroup(BString,bool);
					
					void		RemoveChannelFromUI(entry_ref nref,SubscriptionListItem*);
								
					void LoadColumnsState(BMessage* msg);
					void SaveColumnsState(BMessage* msg);
										
					
					GroupItem* LoadExtraState(BMessage* msg, SubscriptionListItem*);
					void SaveExtraState(BMessage* msg, SubscriptionListItem*);
					
					void	AlertBadPlayer(int32 player_id);
					
					
					MainWindow*	fView;
					MainModel*	fModel;
					Notifier			freeNotifier; //di comodo!
					
					int32		player_id;
					int32		browser_id;
					
					// timer
					BMessageRunner*	check_timer;
					bigtime_t	check_time;
					bool			check_onstart;
					//BString extraParams;	
					
					
										void	AddEpisode(BEntry);
					
					struct ParseParams {
							entry_ref	nref;
							MainController*	fController;
					};
					
					//void		SaveCurrentChannelInfo();
					//MIX
					BString	selectedChannel;
					std::set<thread_id> fValidThreads;
					
					bool	IsValidThread(thread_id currentThread) const;
					void				ParseChannel(entry_ref nref);
					static		int32	ParsingThread(void* data);
};

#endif
