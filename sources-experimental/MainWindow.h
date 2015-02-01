#ifndef 	MainWindow_H
#define MainWindow_H
 
#include <Window.h>
#include <MessageRunner.h>
#include <PictureButton.h>
#include <Entry.h>
#include <TextView.h> 
#include <PopUpMenu.h>
//#include <StackedViewManager.h>
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



 	public:
					 		MainWindow();
				void 		init(MainController* controller);
					 		
		 		 void		MessageReceived(BMessage* message);
				 bool 	QuitRequested();
	
				
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
				float		CreateMenuBar();
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
    	
	 	   		BRect podderrect;
	    
};

#endif
