#ifndef 	MainWindow_H
#define MainWindow_H

#include <FilePanel.h>
#include <Window.h>
#include <MessageRunner.h>
#include <PictureButton.h>
#include <Entry.h>
#include <TextView.h> 
#include <PopUpMenu.h>
#include "StackedView.h"
#include <Box.h>


#include "SubscriptionListView.h"
#include "DownloadListView.h"
#include "EpisodeListView.h"
#include "EpisodeListItem.h"

#include "BPAlert.h"

class SubscriptionListItem;
class SplitPane;
class RunView;

#define ARCHIVE_PARSE	'pars'
#define CHECK_ALL			'chal'

class MemoryArchive;
class MainController;
class DownloadListItem;
class SectionSelector;
class GroupItem;
class OPMLTree;


class MainWindow: public BWindow
{

	enum channelsItemAction {
		CHANNEL_ADD_REQUEST = 0,
		CHANNEL_REMOVE,
		CHANNEL_CHECK,
		CHANNEL_CHECK_ALL,
		CHANNEL_WWW,
		CHANNEL_ENCLOSURE_FOLDER,
		CHANNEL_SHOW_IMAGE,
		CHANNEL_ITEM_ACTION_SIZE
	};

	enum episodesItemAction {
		EPISODE_PLAY = 0,
		EPISODE_DOWNLOAD,
		EPISODE_STOP,
		EPISODE_REMOVE,
		EPISODE_WWW,
		EPISODE_CHANNEL_ENCLOSURE_FOLDER,
		EPISODE_ITEM_ACTION_SIZE
	};

	enum groupsItemAction {
		GROUP_ADD = 0,
		GROUP_REMOVE,
		GROUP_RENAME,
		GROUP_CHECK,
		GROUP_ITEM_ACTION_SIZE
	};


 	public:
					 		MainWindow();
				void 		init(MainController* controller);
					 		
				void		MessageReceived(BMessage* message);
				bool 		QuitRequested();
		virtual void 		MenusBeginning();
	
				
				void			AddGroup(GroupItem*,bool expanded);
				BString		GroupOf(SubscriptionListItem* row);
				
				
				void	AddEpisodeListItem(EpisodeListItem* ep);

				void 	ClearEpisodeListView();
										
				void	AddSubscription(SubscriptionListItem* row,GroupItem* grp);
				
				void 	ScrollToSubscripion(SubscriptionListItem* row);
				
				void	RefreshSubscriptionLists();
				void	RefreshSubscription(SubscriptionListItem* row);
				
				void	RemoveSubscription(SubscriptionListItem* row);
				
				EpisodeListView*		GetEpisodeListView();
				EpisodeListItem*		GetSelectedEpisode(EpisodeListItem* prev = NULL); //indipendentemente dalla view su cui è posto!
				
				
				void	RefreshEpisode(EpisodeListItem* row);
				
				void	ShowDescription(MemoryArchive* archive);
				
				void	SetController(MainController* c){ fController = c;}
				
				SubscriptionListItem*	GetSelectedSubscription();
				
				GroupItem*	GetSelectedGroup();
				bool			GetScreenGroupRect(GroupItem *row, BRect *outRect);
				
				void			RemoveGroup(GroupItem*);
				
				int32 CountSubscriptionOfGroup(GroupItem*);
				
				SubscriptionListItem* GetSubscriptionOfGroup(int32,GroupItem*);
				
				
				DownloadListItem*	GetSelectedDownload();
				
				int32	IndexOfSubscription(SubscriptionListItem* item);
				
				void SetSortingSubscriptions(bool value);
				
				
				void	SelectSubscription(int i);
				void  SelectSubscription(SubscriptionListItem* elv);
				
				void  SelectedSubscription(SubscriptionListItem* elv, int32 bottons = -1);
				
				void	SelectGroup(GroupItem*, int32 bottons = -1);
				//void	SelectEpisode(EpisodeListView* view, EpisodeListItem* item);
				
				
				void				AddDownload(DownloadListItem*);
				void				RemoveDownload(DownloadListItem*);	 

	
				
				void	SaveSetting(BMessage* data);
				void	LoadSetting(BMessage* data);
	private:				
				float	CreateMenuBar();
				BView*	CreateItemInfoView();			
				BView*	CreateChannelInfoView();
				
				void	ShowItemDescription(MemoryArchive* archive);
				void	ShowChannelDescription(MemoryArchive* archive);
				
				
				void	SetFullscreen(bool);
				void	SetToolbarVisible(bool);
				
				
				void	AddDirectoryItem(BMenu*,const char *name,const char* url,char shortcut);
					
				void	RecursiveParseOpml(OPMLTree* branch);
				
				
				MainController*	fController;
									
				BMenuItem*			notoolbar;
				BMenuItem*			fullscreenitem;
				BMenu*					setworkspace;
				BMenu* ChannelSize;
				
				BMenu*					channels;
				BMenu*					items;
				BMenu*					groups;

				BMenuItem*				fGroupItems[GROUP_ITEM_ACTION_SIZE];
				BMenuItem*				fChannelItems[CHANNEL_ITEM_ACTION_SIZE];
				BMenuItem*				fEpisodeItems[EPISODE_ITEM_ACTION_SIZE];

				BView* channelView;	//toolbar
				BView* itemsView;		//toolbar

				
				
				SubscriptionListView 			*sx_list;
				DownloadListView				*down_list;
				
				BBox									*view; //the main view.
				
				StackedView						*fStack;
				EpisodeListView				*theStack;
				
				RunView								*fItemText;
				RunView								*fChannelText;
				
				SplitPane							*split,*dxsplit;
	
				BPopUpMenu*					fChannelMenu;
    			BPopUpMenu*					fItemMenu;
    			BPopUpMenu*					fDownloadMenu;
    			BPopUpMenu*					fGroupMenu;
    			
				SectionSelector*					fSelector;

				BFilePanel 					*importFilePanel;
				BFilePanel 					*exportFilePanel;

	 	   		BRect podderrect;
	    
};

#endif
