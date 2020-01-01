#include "MainWindow.h"
#include "Application.h"
#include "BPSettingsWindow.h"
#include "AddWindow.h"
#include "DirectoryWindow.h"
#include <Catalog.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Alert.h>
#include <Button.h>
#include <DateFormat.h>
#include <DateTimeFormat.h>
#include <View.h>
#include <Box.h>
#include <LayoutBuilder.h>
#include <PathFinder.h>
#include <ScrollBar.h>
#include <StorageKit.h>
#include <StringForSize.h>
#include "ColumnListView.h"
#include "ColumnTypes.h"

#include "EpisodeListItem.h"
#include "SubscriptionListItem.h"

#include "SplitPane.h"
#include "Utils.h"

#include "MemoryArchive.h"
#include "htmlparse.h"
#include <Bitmap.h>
#include "DownloadManager.h"
 
#include "IconTextRender.h" 
#include "TitleTextRender.h"
#include "GreenBoxTextRender.h"

#include "curl/curl.h"
#include <parsedate.h>

#include "ItemRunView.h"
#include "Theme.h"

#include "RVActionBP.h"
#include "RVActionBPOpenURL.h"

#include "DownloadListView.h"
#include "DownloadListItem.h"
#include "MainController.h"
#include "IActionManagerBP.h"
#include "GroupItem.h"

#include "IABPGroup.h"
#include "OPMLParser.h"
#include "OPMLWriter.h"

//messages:
	#define		DOWNLOAD_ITEM				'invD'
	#define		DOWNLOAD_CHANNEL			'addu'


	#define 	ITEM_SELECTED					'selD'
	#define		SET_FULLSCREEN				'full'
	#define 	SHOW_SETTINGS				'setp'
	#define		SHOW_TOOLBAR					'notb'
	#define		SECTION_SELECTED			'sese'
	
	#define		SHOW_HELP			'help'
	#define		WEB_PAGE				'webp'
	
	#define		SHOW_DIRECTORY		'dish'
	
	#define		IMPORT_OPML		'imop'
	#define		EXPORT_OPML		'exop'
	#define 	PARSE_OPML		'paop'
	#define     WRITE_OPML		'wopm'

// Singletons:

extern IActionManagerBP		action_manager;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


MainWindow::MainWindow()
	:
	BWindow(BRect(100,100,970,607), B_TRANSLATE_SYSTEM_NAME("BePodder"),
		B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS),
	importFilePanel(NULL),
	exportFilePanel(NULL)
{

	BBitmap* folder=new BBitmap(BRect(0,0,15,15), 0, B_RGBA32);
	BBitmap* image=new BBitmap(BRect(0,0,15,15), 0, B_RGBA32);

	BMimeType type("application/x-vnd.Be-directory");
	type.GetIcon(folder,B_MINI_ICON);

	BMimeType type2("image");
	type2.GetIcon(image,B_MINI_ICON);

	AddIcon("folder",folder);
	AddIcon("image",image);
}

void
MainWindow::init(MainController* controller){	

	SetController(controller);

    rgb_color bgcolor = ui_color(B_PANEL_BACKGROUND_COLOR);

	const float toolBarHeight = 42.0;

	BRect rect(Bounds());
	rect.top = CreateMenuBar() + 1;

	view=new BBox(rect,NULL, B_FOLLOW_ALL, B_WILL_DRAW,B_NO_BORDER);
	BRect toolBarRect(0, 0, rect.Width(), toolBarHeight);
	toolBarRect.OffsetBy(0,4);

	fToolBar= new BToolBar(toolBarRect);

	AddChild(view);
	view->AddChild(fToolBar);
	const BFont *font = be_plain_font;

	fChannelMenu = new BPopUpMenu("ChannelMenu");
	fChannelMenu->SetFont(font);

	const	 int COUNT = 4;
	const int	ChannelBar[COUNT] = 
					{
						IACTION_CHANNEL_ADD_REQUEST,
						IACTION_CHANNEL_REMOVE,
						IACTION_CHANNEL_CHECK,
						IACTION_CHANNEL_WWW
					};

	fToolBar->GetLayout()->AddItem(BSpaceLayoutItem::CreateHorizontalStrut(3.0));
	//automatic for the people
	int i;
	for(i=0;i<COUNT;i++){
		IAction* action = action_manager.GetAction(ChannelBar[i]);
		fToolBar->AddAction(action->CopyMessage(),IActionManager::TargetForAction(0),
		action->GetIcon(IActionBP::SIZE_24), action->GetDescription().String(),"",false);
	}
	fToolBar->AddGlue();
	//extras :)
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ADD_REQUEST));
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_REMOVE));
	fChannelMenu->AddSeparatorItem();
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_CHECK));
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_CHECK_ALL));
	fChannelMenu->AddSeparatorItem();
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_WWW));
	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ENCLOSURE_FOLDER));
//	fChannelMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_SHOW_IMAGE));
	fChannelMenu->SetTargetForItems(be_app);

	channels->AddItem(fChannelItems[CHANNEL_ADD_REQUEST] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ADD_REQUEST));
	channels->AddItem(fChannelItems[CHANNEL_REMOVE] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_REMOVE));
	channels->AddSeparatorItem();
	channels->AddItem(fChannelItems[CHANNEL_CHECK] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_CHECK));
	channels->AddItem(fChannelItems[CHANNEL_CHECK_ALL] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_CHECK_ALL));
	channels->AddSeparatorItem();
	channels->AddItem(fChannelItems[CHANNEL_WWW] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_WWW));
	channels->AddItem(fChannelItems[CHANNEL_ENCLOSURE_FOLDER] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ENCLOSURE_FOLDER));
//	channels->AddItem(fChannelItems[CHANNEL_SHOW_IMAGE] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_SHOW_IMAGE));
	channels->SetTargetForItems(be_app);

	fItemMenu = new BPopUpMenu("ItemMenu");
	fItemMenu->SetFont(font);

	const	 int COUNT2 = 4;
	const int ItemBar[COUNT2] = 
					{
						IACTION_ITEM_PLAY,
						IACTION_ITEM_DOWNLOAD,	
						IACTION_ITEM_STOP,
						IACTION_ITEM_REMOVE,	
					};

	//automatic for the people
	for(i=0;i<COUNT2;i++){
		fItemMenu->AddItem(action_manager.CreateMenuItemFromAction(ItemBar[i]));
		items->AddItem(fEpisodeItems[i] = action_manager.CreateMenuItemFromAction(ItemBar[i]));

		IAction* action = action_manager.GetAction(ItemBar[i]);
		fToolBar->AddAction(action->CopyMessage(),IActionManager::TargetForAction(0),
		action->GetIcon(IActionBP::SIZE_24), action->GetDescription().String(),"",false);
	}	
	fToolBar->AddGlue();
	//extras :)
	fItemMenu->AddSeparatorItem();
	fItemMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_ITEM_WWW));
	fItemMenu->AddItem(action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ENCLOSURE_FOLDER));

	items->AddSeparatorItem();
	items->AddItem(fEpisodeItems[i++] = action_manager.CreateMenuItemFromAction(IACTION_ITEM_WWW));
	items->AddItem(fEpisodeItems[i++] = action_manager.CreateMenuItemFromAction(IACTION_CHANNEL_ENCLOSURE_FOLDER));

	fItemMenu->SetTargetForItems(be_app);
	items->SetTargetForItems(be_app);
	
	// GROUPS MENU
	
	fGroupMenu = new BPopUpMenu("GroupsMenu");
	fGroupMenu->SetFont(font);
	const	 int COUNT4 = 4;
	const int groupBar[COUNT4] = 
					{
						IACTION_GROUP_ADD,
						IACTION_GROUP_REMOVE,
						IACTION_GROUP_RENAME,
						IACTION_GROUP_CHECK,
					};

	//automatic for the people
	for(i=0;i<COUNT4;i++){
		fGroupMenu->AddItem(action_manager.CreateMenuItemFromAction(groupBar[i]));
		groups->AddItem(fGroupItems[i] = action_manager.CreateMenuItemFromAction(groupBar[i]));
	}	

	groups->SetTargetForItems(be_app);
	fGroupMenu->SetTargetForItems(be_app);
	
	//DOWNLOAD MENU
	
	
	fDownloadMenu = new BPopUpMenu("DownMenu");
	fDownloadMenu->SetFont(font);
	
	const	 int COUNT3 = 5 - 1; // remove unimplemented find item action
	const int downBar[COUNT3] = 
					{
						IACTION_DOWNLOAD_PLAY,
						IACTION_DOWNLOAD_STOP,	
						IACTION_DOWNLOAD_DOWNLOAD,
//						IACTION_DOWNLOAD_FIND_ITEM,
						IACTION_DOWNLOAD_REMOVE
					};
					
					
	//automatic for the people
	for(i=0;i<COUNT3;i++){
		fDownloadMenu->AddItem(action_manager.CreateMenuItemFromAction(downBar[i]));
	}	
	
	fDownloadMenu->SetTargetForItems(be_app);

	//--------------------------------------------------------451

	BRect bounds = view->Bounds();
	
	theStack = new EpisodeListView(BRect(451,52,840,300));
	
	BMessage* msg=new BMessage(DOWNLOAD_ITEM);
	theStack->SetInvocationMessage(msg);
	
	msg=new BMessage(ITEM_SELECTED);
	msg->AddInt32("buttons",0);
	theStack->SetSelectionMessage(msg);
	
		
	sx_list=new SubscriptionListView(BRect(0,0,210,377));
	BView *multyView = new BView("MultyView", B_WILL_DRAW);

	BRect k = Bounds();

	dxsplit =new SplitPane(BRect(451,52,860, bounds.bottom - 10),theStack,multyView,B_FOLLOW_ALL);
	dxsplit->SetViewInsetBy(BPoint(0,0));
	dxsplit->SetAlignment(B_HORIZONTAL);
	dxsplit->SetViewColor(bgcolor);
	dxsplit->SetBarPosition(BPoint(k.Width()/2.0f-170, k.Height()/2.0f+38));
	dxsplit->SetMinSizeOne(BPoint(0,84));
	dxsplit->SetMinSizeTwo(BPoint(0,161));
		
	split =new SplitPane(BRect(10,52,860,bounds.bottom - 10),sx_list,dxsplit,B_FOLLOW_ALL);
	split->SetViewInsetBy(BPoint(0,0));
	split->SetViewColor(bgcolor);
	split->SetBarPosition(BPoint(k.Width()/2.0f-170, k.Height()/2.0f));
	split->SetMinSizeOne(BPoint(200,0));
	split->SetMinSizeTwo(BPoint(200,0));
	
	view->AddChild(split);

	BTabView* tabView = new BTabView("tabview", B_WIDTH_FROM_LABEL);

	BLayoutBuilder::Group<>(multyView, B_VERTICAL, 0)
			.Add(tabView);

	BView*	fakeView=new BView(multyView->Bounds(),B_TRANSLATE("Downloads"),B_FOLLOW_ALL,B_WILL_DRAW);
	fakeView->AddChild(down_list=new DownloadListView(fakeView->Bounds()));

	BView *xview;
	tabView->AddTab(xview = CreateItemInfoView());
	xview->SetExplicitMinSize(BSize(0,0));
	tabView->AddTab(xview = CreateChannelInfoView());
	xview->SetExplicitMinSize(BSize(0,0));
	tabView->AddTab(fakeView);
	fakeView->SetExplicitMinSize(BSize(0,0));
	UpdateToolBar();
}


BView*
MainWindow::CreateItemInfoView(){
	//RUN VIEW--------------------------------------------------------
		
	 Theme *fTheme = new Theme("ItemInfo_view", MAX_COLORS + 1, MAX_COLORS + 1, MAX_FONTS + 1);
		
	 fTheme->WriteLock();
	 fTheme->SetForeground(C_URL, 0, 13, 180);     //0, 100, 180
	 fTheme->SetBackground(C_URL, 255, 255, 255);
	 fTheme->SetForeground(C_TEXT, 0, 0, 0);
	 fTheme->SetBackground(C_TEXT, 255, 255, 255);
	 fTheme->SetForeground(C_SELECTION, 255, 255, 255);
	 fTheme->SetBackground(C_SELECTION, 0, 0, 0);
	 
	 fTheme->SetForeground(C_ACTION,greyBox.red,greyBox.green,greyBox.blue);
	 fTheme->SetBackground(C_ACTION,greyBox.red,greyBox.green,greyBox.blue);
	 
	 IconTextRender *itr;
	 fTheme->SetTextRender(F_EMOTICON,itr=new IconTextRender());
	 
	 itr->AddIcon("folder ",LoadIcon("folder"));
	 itr->SetIconSize(16.0,16.0);
	 
	 fTheme->SetTextRender(F_ACTION,new TitleTextRender());
	 fTheme->SetTextRender(F_TIMESTAMP,new GreenBoxTextRender());
	 
	 
	 
	//  fTheme->SetTextRender(F_STYLE_0,btr = new BitmapTextRender());
	 	 
	fTheme->WriteUnlock();
	
	BRect scrollRect(0,0,100,100);

	fItemText = new ItemRunView(scrollRect, "text", fTheme,B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE);
	
	BScrollView *fTextScroll = new BScrollView(B_TRANSLATE("Episode info"), fItemText, 0,false, true, B_PLAIN_BORDER);
	
	
	fItemText->AddAction("bepodder",new RVActionBP());
	fItemText->SetDefaultOpenURLAction(new RVActionBPOpenURL(fController));
	
	return fTextScroll;
}

BView*
MainWindow::CreateChannelInfoView(){
	 		
	 Theme *fTheme = new Theme("ChannelInfo_view", MAX_COLORS + 1, MAX_COLORS + 1, MAX_FONTS + 1);
		
	 fTheme->WriteLock();
	 
	 fTheme->SetForeground(C_URL, 0, 13, 180);    //blue?  //0, 100, 180
	 fTheme->SetBackground(C_URL, 255, 255, 255); //white
	 
	 fTheme->SetForeground(C_TEXT, 0, 0, 0);
	 fTheme->SetBackground(C_TEXT, 255, 255, 255);
	 
	 fTheme->SetForeground(C_SELECTION, 255, 255, 255);
	 fTheme->SetBackground(C_SELECTION, 0, 0, 0);
	 
	 fTheme->SetForeground(C_ACTION,greyBox.red,greyBox.green,greyBox.blue);
	 fTheme->SetBackground(C_ACTION,greyBox.red,greyBox.green,greyBox.blue);
	 
	 IconTextRender *itr;
	 fTheme->SetTextRender(F_EMOTICON,itr=new IconTextRender());
	 
	 BBitmap*	bitmap = LoadIcon("feed.png");
	 itr->AddIcon("feed ",bitmap);
	 itr->SetIconSize(bitmap->Bounds().Height(),bitmap->Bounds().Width());
	 
	fTheme->SetTextRender(F_ACTION,new TitleTextRender());
	fTheme->SetTextRender(F_TIMESTAMP,new GreenBoxTextRender());
	 	 
	fTheme->WriteUnlock();
	
	BRect scrollRect(0,0,100,100 );
	
	fChannelText = new RunView(scrollRect, "channel ", fTheme,B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE);
	BScrollView *fTextScroll = new BScrollView(B_TRANSLATE("Subscription info"), fChannelText, 0,false, true, B_PLAIN_BORDER);
		
	fChannelText->AddAction("bepodder",new RVActionBP());
	fChannelText->SetDefaultOpenURLAction(new RVActionBPOpenURL(fController));
	
	return fTextScroll;
	

}

float
MainWindow::CreateMenuBar(){

	BMenuBar *poddermenubar = new BMenuBar(BRect(0,0,1,1),"Poddermenubar");
	BMenu  *podderfile = new BMenu(B_TRANSLATE("File"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(podderfile);
	BMenuItem *podderaboutitem = new BMenuItem(B_TRANSLATE("About BePodder"),new BMessage(B_ABOUT_REQUESTED),0,0);

	podderaboutitem->SetTarget(be_app);
	podderfile->AddItem(podderaboutitem);
	podderfile->AddItem(new BMenuItem(B_TRANSLATE("Show documentation"), new BMessage(SHOW_HELP),0,0));
	podderfile->AddSeparatorItem();
	podderfile->AddItem(new BMenuItem(B_TRANSLATE("Import OPML" B_UTF8_ELLIPSIS) , new BMessage(IMPORT_OPML), 0, 0));
	podderfile->AddItem(new BMenuItem(B_TRANSLATE("Export OPML" B_UTF8_ELLIPSIS) , new BMessage(EXPORT_OPML), 0, 0));
	podderfile->AddSeparatorItem();
	podderfile->AddItem(new BMenuItem(B_TRANSLATE("Preferences" B_UTF8_ELLIPSIS),new BMessage(SHOW_SETTINGS),',',0));
	podderfile->AddSeparatorItem();
	BMenuItem *quititem = new BMenuItem(B_TRANSLATE("Quit"),new BMessage(B_QUIT_REQUESTED),'Q',0);
	quititem->SetTarget(be_app);
	podderfile->AddItem(quititem);

	groups = new BMenu(B_TRANSLATE("Groups"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(groups);
	
	channels = new BMenu(B_TRANSLATE("Subscriptions"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(channels);
	
	items = new BMenu(B_TRANSLATE("Episodes"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(items);	
	/*  TODO create a List of rss feeds

	BMenu  *directoryfile = new BMenu(B_TRANSLATE("Lists"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(directoryfile);

	AddDirectoryItem(directoryfile,B_TRANSLATE("Podcast List"),"http://www.funkyideasoft.com/directories/bepodder_podcasts.opml.tar.gz",'L');
	AddDirectoryItem(directoryfile,B_TRANSLATE("Videoblog List"),"http://www.funkyideasoft.com/directories/bepodder_videoblog.opml.tar.gz",'M');
	AddDirectoryItem(directoryfile,B_TRANSLATE("Imagefeeds List"),"http://www.funkyideasoft.com/directories/bepodder_imagefeeds.opml.tar.gz",'I');
	AddDirectoryItem(directoryfile,B_TRANSLATE("Newsfeeds List"),"http://www.funkyideasoft.com/directories/bepodder_newsfeeds.opml.tar.gz",'N');
	AddDirectoryItem(directoryfile,B_TRANSLATE("BitTorrentfeeds List"),"http://www.funkyideasoft.com/directories/bepodder_bittorrentfeeds.opml.tar.gz",'B');
	AddDirectoryItem(directoryfile,B_TRANSLATE("Internationalfeeds List"),"http://www.funkyideasoft.com/directories/bepodder_internationalfeeds.opml.tar.gz",'J');
	*/

	BMenu  *podderview = new BMenu(B_TRANSLATE("View"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(podderview);
	
	fullscreenitem = new BMenuItem(B_TRANSLATE("Full screen"),new BMessage(SET_FULLSCREEN), B_ENTER, 0);
	podderview->AddItem(fullscreenitem);
	fullscreenitem->SetMarked(false);
	
	notoolbar= new BMenuItem(B_TRANSLATE("Toolbar"),new BMessage(SHOW_TOOLBAR),'B',0);
	podderview->AddItem(notoolbar);
	notoolbar->SetMarked(true);

	/* TODO implement small size BRow doesn't support SetHeight
	ChannelSize = new BMenu(B_TRANSLATE("Subscriptions View"));
	podderview->AddItem(ChannelSize);
	ChannelSize->SetRadioMode(true);
	
	BMenuItem* temp;
	
	temp= new BMenuItem(B_TRANSLATE("Big items"),new BMessage('bigg'));
	ChannelSize->AddItem(temp);
	temp->SetMarked(true);
	
	temp=new BMenuItem(B_TRANSLATE("Small items"),new BMessage('smal'));
	ChannelSize->AddItem(temp);
	temp->SetMarked(false);
	
	ChannelSize->SetEnabled(true);	
	*/
	
	AddChild(poddermenubar);
	poddermenubar->ResizeToPreferred();
	
	return poddermenubar->Bounds().bottom;
}

void
MainWindow::AddDirectoryItem(BMenu* directoryfile,const char *name,const char* url,char shortcut) {
	
	BMessage* msg=new BMessage(SHOW_DIRECTORY);
	msg->AddString("name",name);
	msg->AddString("url",url);
	
	directoryfile->AddItem(new BMenuItem(name,msg,shortcut,0));
}

void
MainWindow::SaveSetting(BMessage* data){
	
	BRect rect(Frame());
	
	if(fullscreenitem->IsMarked())
			rect = podderrect;
	
	data->AddFloat("window_x",rect.left);
	data->AddFloat("window_y",rect.top);
	data->AddFloat("window_w",rect.Width());
	data->AddFloat("window_h",rect.Height());
	
	//splits
	BMessage split_state;
	split->GetState(split_state);
	data->AddMessage("bar_position_state",&split_state);
	
	split_state.MakeEmpty();
	dxsplit->GetState(split_state);
	data->AddMessage("dxbar_position_state",&split_state);
	
	//toolbar (visible?)
	data->AddBool("toolbar_visible",notoolbar->IsMarked());
		
	//fullscreen (on?)	    	
	data->AddBool("fullscreen",fullscreenitem->IsMarked());
	
	
	//default Subscription Items size:
	data->AddInt32("channel_size",sx_list->ItemSize());    	
	
	//debug..
	//data->PrintToStream();
	

}


void 
MainWindow::LoadSetting(BMessage* data){

		//data->PrintToStream();
		
		float x,y;
		if(data->FindFloat("window_x",&x)==B_OK && data->FindFloat("window_y",&y)==B_OK){
			MoveTo(x,y);
			podderrect.left=x;
			podderrect.top=y;
		}
		if(data->FindFloat("window_w",&x)==B_OK && data->FindFloat("window_h",&y)==B_OK){
			ResizeTo(x,y);
			podderrect.right=podderrect.left + x - 1;
			podderrect.bottom=podderrect.top + y - 1;	
		}
				
		//fullscreen (on?)
		bool result;	    	
		if(data->FindBool("fullscreen",&result) == B_OK){
			if(result)
				SetFullscreen(result);
		}
		
		//toolbar (visible?)
		if(data->FindBool("toolbar_visible",&result) == B_OK){
			if(!result)
				SetToolbarVisible(result);
		}
		
		//splits		
		BMessage split_state;
		if(data->FindMessage("bar_position_state",&split_state) == B_OK){
			split->SetState(&split_state);
		}
		split_state.MakeEmpty();
		if(data->FindMessage("dxbar_position_state",&split_state) == B_OK){
			dxsplit->SetState(&split_state);
		}
		/* TODO FIX Row Size
		int32 size;
		//default Subscription Items size:
		if(data->FindInt32("channel_size",&size) == B_OK){
			sx_list->SetItemSize((SubscriptionSize)size);
			ChannelSize->ItemAt(size)->SetMarked(true);
		}    	

		ChannelSize->SetEnabled(true); */
		sx_list->SetItemSize(BIG);

}


void MainWindow::MessageReceived(BMessage* msg)
{
	
	switch(msg->what)
	{

		case 'bigg':
			sx_list->SetItemSize(BIG);
		break;
		case 'smal':
			sx_list->SetItemSize(SMALL);
		break;
		
		case SHOW_SETTINGS:
		{
			BPSettingsWindow *Settings = new BPSettingsWindow();
			Settings->Show();
		} 
		break;
		case SHOW_DIRECTORY:
		{
				BString name;
				BString url;
				msg->FindString("name",&name);
				msg->FindString("url",&url);
				DirectoryWindow *Directory = new DirectoryWindow(name.String(),url);
				Directory->Show();
		}
		break;
		
		case WEB_PAGE:
		{
			BString url;
			if(msg->FindString("url",&url) == B_OK)	
				fController->OpenURL(url.String());
		}
		break;

		case SHOW_HELP:
		{
			_ShowDocumentation();
		} 
		break;

		case SET_FULLSCREEN:
			SetFullscreen(!fullscreenitem->IsMarked());
		break;
				
		case CHECK_ALL:
			fController->CheckAllChannel();
		break;
		
		case ARCHIVE_PARSE:
		{
			//show the alert
			BPAlert *alert = new BPAlert("BePodder",B_TRANSLATE("\nParsing archive" B_UTF8_ELLIPSIS),NULL,NULL,NULL,B_WIDTH_AS_USUAL, LoadIcon("enqueued-32.png"));
			alert->Go(NULL);
			

			fController->ParseArchive(); 
					
			if(sx_list->CountRows()>0) {
				sx_list->AddToSelection(sx_list->RowAt(0));
				SubscriptionListItem*	row=dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(0));
				if(row)
				fController->SetSelectedItem( NULL,&row->fRef);	
			}
			//close the alert
			alert->PostMessage(B_QUIT_REQUESTED);
		}
		break;
		
		case DOWNLOAD_CHANNEL: 
			fController->DownloadChannel(msg);
		break;
		
		case DOWNLOAD_CHANNEL_INFO:
			fController->InfoDownloadChannel(msg);
		break;
		
		case DOWNLOAD_ITEM_INFO: 
			fController->InfoDownloadItem(msg);		
		break;
		
		case CHANNEL_SELECTED:
		{	
			SubscriptionListItem* row=dynamic_cast<SubscriptionListItem*>(sx_list->CurrentSelection());
			if(row) 
				SelectedSubscription(row,msg->FindInt32("buttons"));
			
			GroupItem*	group=dynamic_cast<GroupItem*>(sx_list->CurrentSelection());
			if(group)
					SelectGroup(group,msg->FindInt32("buttons"));
		}	
		break;
		
		case ITEM_SELECTED:
		{
			
			SubscriptionListItem*	row=GetSelectedSubscription();
			if(!row) return;
			
			EpisodeListItem*	episode=GetSelectedEpisode();
			if(!episode) return;
							
			fController->SetSelectedItem(&episode->fRef,&row->fRef);
			
			
			EpisodeListView *elv=theStack; //FIX FOR GROUP MANAGE
			if(!elv) return;
			
			if(msg->FindInt32("buttons")==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				elv->ScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = elv->ScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fItemMenu->FindMarked())
					fItemMenu->FindMarked()->SetMarked(false);
				
				elv->SelectionMessage()->ReplaceInt32("buttons",0);
				fItemMenu->Go(p2, true, true, true);
			
			}
			UpdateToolBar();
		}
		break;
		
		case DOWNLOAD_SELECTED:
		{
						
			if(msg->FindInt32("buttons")==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				down_list->ScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = down_list->ScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fDownloadMenu->FindMarked())
					fDownloadMenu->FindMarked()->SetMarked(false);
				
				fDownloadMenu->Go(p2, true, true, true);
			
			}
			
			down_list->ResetSelectionMessage();
		}
		break;
		case SECTION_SELECTED:
			ShowDescription((MemoryArchive*)fController->GetSelectedItem());
		break;
		case DOWNLOAD_INVOKED:
				action_manager.FireAction(IACTION_DOWNLOAD_DOWNLOAD);
		break;
		case DOWNLOAD_ITEM: 
			action_manager.FireAction(IACTION_ITEM_DOWNLOAD);
		break;
		
		case DOWNLOAD_EXTRA_INFO:
			fController->InfoExtraDownload(msg);
		break;
		
		case SHOW_TOOLBAR:
			SetToolbarVisible(!notoolbar->IsMarked());
		break;
		
		case GROUP_NAME_CHANGE:
		{
			BString oldname;
			BString newname;
			if( msg->FindString("oldname",&oldname) ==B_OK &&
				msg->FindString("_value",&newname)==B_OK )
					fController->RenameGroup(oldname,newname);
		}
		break;
		
		case IMPORT_OPML:
		{
			// select the OPML file,
			if (importFilePanel == NULL) {
				importFilePanel = new BFilePanel(B_OPEN_PANEL);
				importFilePanel->SetMessage(new BMessage(PARSE_OPML));
				importFilePanel->SetTarget(BMessenger(this));
			}
			importFilePanel->Show();
			break;
		}
		case PARSE_OPML:
		{
			entry_ref ref;
			
			if(msg->FindRef("refs",&ref) == B_OK)
			{
				OPMLParser parser;
				OPMLTree*	tree = parser.Parse(BPath(&ref).Path());
				
				if(!tree) return; //should never happen!
						
				RecursiveParseOpml(tree);
			}			
			msg->PrintToStream();
		}
		break;

		case EXPORT_OPML:
		{
			// select the OPML file,
			if (exportFilePanel == NULL) {
				exportFilePanel	= new	BFilePanel(B_SAVE_PANEL);
				exportFilePanel->SetMessage(new BMessage(WRITE_OPML));
				exportFilePanel->SetTarget(BMessenger(this));
				exportFilePanel->SetSaveText("Podcast.opml");
			}
			exportFilePanel->Show();
			break;
		}
		case WRITE_OPML:
		{
			entry_ref ref;
			BString fileName;
			if(msg->FindRef("directory",&ref) == B_OK && msg->FindString("name",&fileName) == B_OK)
			{
				BDirectory directory(&ref);
				BPath exportFilePath(&directory, fileName.String());
				OPMLWriter writer("Feed BePodder");

/*
				for (int i = 0; i < sx_list->CountRows(); i++)
				{
					writer.AddChannel(dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(i)));
				}
				writer.WriteToFile(exportFilePath.Path());
*/
				for(int i = 0; i < sx_list->CountRows(); i++) {
					int numChildren = sx_list->CountRows(sx_list->RowAt(i));
					if (numChildren > 0) {
						//GroupItem* group  = dynamic_cast<GroupItem*>(sx_list->RowAt(i));
						//printf("Group %s\n", group->GroupName().String());
						for (int j = 0; j < numChildren; j++) {
							SubscriptionListItem* subscription = dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(j, sx_list->RowAt(i)));
							//printf("Subscription %s\n", subscription->GetTitle().String());
							writer.AddChannel(subscription);
						}
					}
					else
						writer.AddChannel(dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(i)));
				}
				writer.WriteToFile(exportFilePath.Path());
			}
		}
		break;
		default:
			BWindow :: MessageReceived(msg);
		break;
	}
}

void MainWindow::RecursiveParseOpml(OPMLTree* branch)
{
		if(branch->foglie)
		{
			//it's a group.
			for(int32 i=branch->foglie->CountItems() -1 ; i>=0 ; i--){
				OPMLTree* subfoglia = branch->foglie->ItemAt(i);
				//fController->AddNewGroup(branch->description.String());
				RecursiveParseOpml(subfoglia);
			}

		}
		else 
		{
			//it's a link
			BMessage	msg('addu');
			msg.AddString("url", branch->link);
			BMessenger(this).SendMessage(&msg);
			printf("sending url %s\n", branch->link.String());
		}
}

//-----------------------------------

/*
EpisodeListView*	
MainWindow::CreateEpisodeListView(BString url){
	
//	Lock();
//	BRect rect = fStack->Bounds();
//	Unlock();
//	
//	//BRect rect(0,0,0,0);
//	
//	EpisodeListView* elv=new EpisodeListView(rect);
//	
//	BMessage* msg=new BMessage(DOWNLOAD_ITEM);
//	msg->AddString("url",url);
//	elv->SetInvocationMessage(msg);
//	
//	msg=new BMessage(ITEM_SELECTED);
//	msg->AddString("url",url);
//	msg->AddInt32("buttons",0);
//	elv->SetSelectionMessage(msg);
	
	return theStack;
}*/
/*
void 
MainWindow::AddEpisodeListView(EpisodeListView* elv){
	//fStack->AddView(elv);
}
*/
void
MainWindow::AddEpisodeListItem(EpisodeListItem* ep){
	theStack->AddRow(ep);
}

									
void	
MainWindow::AddSubscription(SubscriptionListItem* row,GroupItem* grp){
	sx_list->AddRow(row ,sx_list->CountRows(),grp);
}
				
void 
MainWindow::ScrollToSubscripion(SubscriptionListItem* row){
	sx_list->ScrollTo(row); //auto-scroll to new item.
}
				
	void	
MainWindow::RefreshSubscriptionLists(){
	sx_list->Refresh();	
}

void	
MainWindow::RefreshSubscription(SubscriptionListItem* row) {
	sx_list->UpdateRow(row);
}

void
MainWindow::AddGroup(GroupItem* group,bool expanded){
	sx_list->AddGroup(group);
	sx_list->ExpandOrCollapse(group,expanded);
}

BString		
MainWindow::GroupOf(SubscriptionListItem* row){
	BRow *parent=NULL;
	sx_list->FindParent(row,&parent,NULL);
	if(!parent) return NULL;
	
	GroupItem* grp=dynamic_cast<GroupItem*>(parent);
	if(!grp) return NULL;
	
	return grp->GroupName();
}

				
				
void	
MainWindow::RemoveSubscription(SubscriptionListItem* row){
	
	sx_list->DeselectAll();
	sx_list->RemoveRow(row);
	if(sx_list->CountRows() > 0)
	{
		sx_list->AddToSelection(sx_list->RowAt(0));
		sx_list->SelectionChanged();
	}	
	else
		ClearEpisodeListView();
		
	delete row;
}


EpisodeListView*		
MainWindow::GetEpisodeListView(){
		return theStack;
}


EpisodeListItem*		
MainWindow::GetSelectedEpisode(EpisodeListItem* prev){ //indipendentemente dalla view su cui è posto!
	return dynamic_cast<EpisodeListItem*>(theStack->CurrentSelection(prev));
}


void	
MainWindow::RefreshEpisode(EpisodeListItem* row) {
	theStack->UpdateRow(row);
}


void	
MainWindow::SelectSubscription(int i){
	
	Lock();
	if(sx_list->CountRows()>0) {
		SubscriptionListItem* sel=dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(i));
		SelectSubscription(sel);
	}
	Unlock();
}

void	
MainWindow::SelectSubscription(SubscriptionListItem* sel){
	sx_list->DeselectAll();
	sx_list->AddToSelection(sel);
	SelectedSubscription(sel);
}
				
void 	
MainWindow::ClearEpisodeListView() {
	theStack->Clear();
}
				
void	
MainWindow::SelectedSubscription(SubscriptionListItem* row, int32 bottons){
			
			if(!row) return;
			
			row->SetIsNew(false);
						
			fController->SwitchChannel(&row->fRef);
			
			if(bottons==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				sx_list->ScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = sx_list->ScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fChannelMenu->FindMarked())
					fChannelMenu->FindMarked()->SetMarked(false);
				
				sx_list->SelectionMessage()->ReplaceInt32("buttons",0);
				fChannelMenu->Go(p2, true, true, true);
		}
		UpdateToolBar();
}

void	
MainWindow::SelectGroup(GroupItem* grp, int32 bottons){
			
			if(!grp) return;
			
			fController->SwitchChannel(NULL);
			
			if(bottons==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				sx_list->ScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = sx_list->ScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fGroupMenu->FindMarked())
					fGroupMenu->FindMarked()->SetMarked(false);
				
				sx_list->SelectionMessage()->ReplaceInt32("buttons",0);
				fGroupMenu->Go(p2, true, true, true);
		}
		UpdateToolBar();
}

void			
MainWindow::RemoveGroup(GroupItem* group){
		sx_list->RemoveRow(group);
		delete group;
}




SubscriptionListItem*	
MainWindow::GetSelectedSubscription(){
		return dynamic_cast<SubscriptionListItem*>(sx_list->CurrentSelection());
}

GroupItem*	
MainWindow::GetSelectedGroup(){
		return dynamic_cast<GroupItem*>(sx_list->CurrentSelection());
}

bool			
MainWindow::GetScreenGroupRect(GroupItem *row, BRect *outRect) {
	BRect rect;
	if(!sx_list->GetRowRect((const BRow *)row, &rect) ) return false;
	*outRect= sx_list->ScrollView()->ConvertToScreen(rect);
	return true;
	
}

int32 
MainWindow::CountSubscriptionOfGroup(GroupItem* grp){
	return sx_list->CountRows(grp);
}
				
SubscriptionListItem* 
MainWindow::GetSubscriptionOfGroup(int32 i,GroupItem* grp){
		return dynamic_cast<SubscriptionListItem*>(sx_list->RowAt(i,grp));
}
				
				
DownloadListItem*	
MainWindow::GetSelectedDownload(){
		return dynamic_cast<DownloadListItem*>(down_list->CurrentSelection());
}
				
int32	
MainWindow::IndexOfSubscription(SubscriptionListItem* item){
		return sx_list->IndexOf((BRow*)item);
}
				
void 
MainWindow::SetSortingSubscriptions(bool value){
			sx_list->SetSortingEnabled(value);
			sx_list->SetSortColumn(sx_list->ColumnAt(0),true,true);
}

void
MainWindow::AddDownload(DownloadListItem* down){
	down_list->AddRow(down,(int32)0);
}

void
MainWindow::RemoveDownload(DownloadListItem* item){
	down_list->RemoveRow(item);
	delete item;
}

				

bool 
MainWindow::QuitRequested() {
	be_app->PostMessage(B_QUIT_REQUESTED);
	return false; 
}


void	
MainWindow::ShowDescription(MemoryArchive* archive){
			ShowItemDescription(archive);
			ShowChannelDescription(archive);
}

				
void	
MainWindow::ShowItemDescription(MemoryArchive* archive){
	
	
	fItemText->Clear();
	((ItemRunView*)fItemText)->LoadBitmap(NULL);
	
	BString itemTitle(archive->GetDataString(CHANNEL_TITLE) );
	BString	webLink(archive->GetDataString(CHANNEL_WEBLINK));
	
	if(itemTitle !="" )	
	{
			if(  webLink != "" )
			fItemText->AppendURL(itemTitle.String(),webLink.String(),C_ACTION,F_TEXT);
		else
			fItemText->Append( itemTitle.String(),C_TEXT,C_ACTION,F_TIMESTAMP );
	}
	time_t	*curtime = 0;
	if(archive->GetData(ITEM_PUBDATE,(const void**)&curtime) >0){
// TODO should be converted in local time?
// if so, time in ColumnView and in the info could be different
// Should be converted in the metadata?

/*		char	dateString[256];
		tm		time_data;
		time_t datetime=*curtime;
		localtime_r(&datetime, &time_data);

		//"%A, %B %d %Y, %I:%M:%S %p"		
		strftime(dateString, 256, "%a, %B %d %Y, %I:%M:%S %p", &time_data);

		fItemText->Append("   - ",C_TEXT,C_ACTION,F_TIMESTAMP);
		fItemText->Append(dateString,C_TEXT,C_ACTION,F_TIMESTAMP);
*/
		BDateTimeFormat formatter;
		BString dateString;
		if (formatter.Format(dateString, *curtime, B_FULL_DATE_FORMAT,
				B_LONG_TIME_FORMAT) == B_OK) {
			fItemText->Append("   - ",C_TEXT,C_ACTION,F_TIMESTAMP);
			fItemText->Append(dateString.String(),C_TEXT,C_ACTION,F_TIMESTAMP);
		}
	} 
	else
		fItemText->Append(" ",C_TEXT,C_ACTION,F_TIMESTAMP);
	
	fItemText->Append("\n",C_TEXT,C_TEXT,F_TEXT);

	if(archive->GetDataString(ITEM_TITLE) !="")	{
		if(archive->GetDataString(ITEM_LINK) != "")
			fItemText->AppendURL(archive->GetDataString(ITEM_TITLE).String(),archive->GetDataString(ITEM_LINK) .String(),F_ACTION);
		else
			fItemText->Append(archive->GetDataString(ITEM_TITLE).String(),C_TEXT,C_TEXT,F_ACTION);
	}

	fItemText->Append("\n",C_TEXT,C_TEXT,F_TEXT);

	const void*	buffer = NULL;
	ssize_t m_size = archive->GetData(ITEM_DESCRIPTION,&buffer);

	if( m_size>0){
		//do what you need to do.
		char* copybuffer=(char*)malloc(m_size);
		memcpy(copybuffer,buffer,m_size);
		
		m_size = parse_html(copybuffer,m_size);
		BString data(copybuffer,m_size);
		fItemText->Append(data.String(),C_TEXT,C_TEXT,F_TEXT);
		
		free(copybuffer);
	}
	BString url = archive->GetDataString(ITEM_ENCLOSURE_URL);
	if( url != ""){
		
					
		
		fItemText->Append("\n\n",C_TEXT,C_TEXT,F_TEXT);
		
		BString action("action://bepodder(");
		action << (int)IACTION_CHANNEL_ENCLOSURE_FOLDER << ")";
		
		fItemText->AppendURL("folder ",action.String(),C_ACTION,F_EMOTICON);
		fItemText->AppendURL(B_TRANSLATE("Enclosure: "),action.String(),C_ACTION,F_TEXT);
				
		BString local(archive->GetDataString(ITEM_ENCLOSURE_LOCALPATH));
		//path.Append(local.String());
		//folder_url.SetTo(path.Path());
		//fItemText->AppendURL(local.String(),folder_url.String(),C_ACTION,F_TEXT);
		
		BString bottom;
		bottom << local;
		
		BString filetype(archive->GetDataString(ITEM_ENCLOSURE_TYPE) );
		if( filetype !="")
			bottom << " (" << filetype<< ") " ;
		
		
		off_t* len = 0;
		if (archive->GetData(ITEM_ENCLOSURE_LENGTH,(const void**)&len)>0 && len>0){
			char sizeString[256];
			bottom << " [" << string_for_size(*len, sizeString, sizeof(sizeString)) << "]";
		}
		fItemText->Append(bottom.String(),C_TEXT,C_ACTION,F_TIMESTAMP);
		//fItemText->Append(" test.mp3 1234Kb (audio/mp3)  downloaded",C_TEXT,C_ACTION,F_TIMESTAMP);
	
	
	
	// AUTO_SHOWER IMAGE.
	//SE: filestatus = DOWNLOADED, SE filetype inzia on image/, SE localpath != "" allora carica e mostra...
	

		FileStatus status = (FileStatus)archive->GetDataInt32(ITEM_ENCLOSURE_FILE_STATUS);
		if( status == DOWNLOADED &&
			filetype.FindFirst("image/")==0 &&
			local != "" )
		{	
			fItemText->Append("\n",C_TEXT,C_TEXT,F_TEXT);
			//Fullpath:
			SubscriptionListItem* row=GetSelectedSubscription();
			if(row){
					BString fullPath=fController->PathFolderChannel(row->fRef);
					fullPath << "/" <<  local;
					((ItemRunView*)fItemText)->LoadBitmap(fullPath.String());
			}
		}

	
	}
	
	fItemText->Append("\n",C_TEXT,C_TEXT,F_TEXT);
	fItemText->ScrollTo(0,0);

}

void	
MainWindow::ShowChannelDescription(MemoryArchive* archive){
	
	//int fItemText;
	
	fChannelText->Clear();
	fChannelText->Append("\n",C_TEXT,C_TEXT,F_TEXT);	
/*
	if(archive->GetDataString(CHANNEL_TITLE) != "")
		fChannelText->Append(archive->GetDataString(CHANNEL_TITLE).String(),C_TEXT,C_TEXT,F_ACTION);

	if(archive->GetDataString(CHANNEL_WEBLINK) != ""){
		fChannelText->AppendURL("  [www]",archive->GetDataString(CHANNEL_WEBLINK) .String(),F_TEXT);
	}
*/
	if(archive->GetDataString(CHANNEL_TITLE) !="")	{
		if(archive->GetDataString(CHANNEL_WEBLINK) != "")
			fChannelText->AppendURL(archive->GetDataString(CHANNEL_TITLE).String(),archive->GetDataString(CHANNEL_WEBLINK) .String(),F_ACTION);
		else
			fChannelText->Append(archive->GetDataString(CHANNEL_TITLE).String(),C_TEXT,C_TEXT,F_ACTION);
	}

	fChannelText->Append("\n",C_TEXT,C_TEXT,F_TEXT);	

	const void *	buffer = NULL;
	ssize_t size = archive->GetData(CHANNEL_DESCRIPTION,&buffer);
		
	if( size>0){
		char* copybuffer=(char*)malloc(size);
		memcpy(copybuffer,buffer,size);
		
		size = parse_html(copybuffer,size);
		BString data(copybuffer,size);
		fChannelText->Append(data.String(),C_TEXT,C_TEXT,F_TEXT);
		
		free(copybuffer);
	}
	/*
	size = archive->GetData(CHANNEL_IMAGE_DATA,(const void**)&buffer);
	
	if( size>0){
		BString action("action://bepodder(");
		action << (int)IACTION_CHANNEL_SHOW_IMAGE << ")";
		fChannelText->Append("\n\n",C_TEXT,C_TEXT,F_TEXT);	
		fChannelText->AppendURL(B_TRANSLATE("Show channel image"),action.String(),C_TEXT,F_TEXT);	
		//free(buffer);
	}
	*/
	fChannelText->Append("\n\n",C_TEXT,C_TEXT,F_TEXT);	
	fChannelText->AppendURL("feed ",archive->GetDataString(CHANNEL_URL) .String(),C_TEXT,F_EMOTICON); //? Icon ??
	fChannelText->AppendURL("XML feed link",archive->GetDataString(CHANNEL_URL) .String(),F_TEXT);
	fChannelText->Append("\n",C_TEXT,C_TEXT,F_TEXT);	
		
	fChannelText->Append("\n",C_TEXT,C_TEXT,F_TEXT);
	fChannelText->ScrollTo(0,0);


}

void
MainWindow::SetFullscreen(bool fullscreen){
			
			BRect wrect(Bounds());
			if (!fullscreen) {
				fullscreenitem->SetMarked(false);
				wrect = podderrect;
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			}	 
			else {
				fullscreenitem->SetMarked(true);
				BScreen screen;
				podderrect = Frame();
				wrect = screen.Frame();
				//BRect frect(Bounds());
				SetFlags(Flags() | B_NOT_RESIZABLE | B_NOT_MOVABLE);
			}
		
			MoveTo(wrect.left, wrect.top);
			ResizeTo(wrect.Width(), wrect.Height());	
			
			BRect frect(Bounds());	
			dxsplit->SetBarPosition(BPoint(frect.Width()/2.0f-170, frect.Height()/2));
}

void
MainWindow::SetToolbarVisible(bool visible){

			BPoint point(0,42);
			if(!visible) {
				notoolbar->SetMarked(false);
				view->ScrollTo(point);
				split->ResizeBy(point.x,point.y);
				dxsplit->SetBarPosition(dxsplit->GetBarPosition()+=point);
				if(!fToolBar->IsHidden())
					fToolBar->Hide();
			}	else {
				notoolbar->SetMarked(true);
				view->ScrollTo(BPoint(0,0));
				split->ResizeBy(point.x,-point.y);
				dxsplit->SetBarPosition(dxsplit->GetBarPosition()-=point);
				if(fToolBar->IsHidden())
					fToolBar->Show();
			}
}

void
MainWindow::MenusBeginning()
{
	GroupItem*	group = GetSelectedGroup();
	fGroupItems[GROUP_REMOVE]->SetEnabled(group != NULL);
	fGroupItems[GROUP_RENAME]->SetEnabled(group != NULL);
	fGroupItems[GROUP_CHECK]->SetEnabled(group != NULL);

	SubscriptionListItem* subscription = GetSelectedSubscription();
	fChannelItems[CHANNEL_REMOVE]->SetEnabled(subscription != NULL);
	fChannelItems[CHANNEL_CHECK]->SetEnabled(subscription != NULL);
	fChannelItems[CHANNEL_WWW]->SetEnabled(subscription != NULL);
	fChannelItems[CHANNEL_ENCLOSURE_FOLDER]->SetEnabled(subscription != NULL);
//	fChannelItems[CHANNEL_SHOW_IMAGE]->SetEnabled(subscription != NULL);

	EpisodeListItem* episode = GetSelectedEpisode();
	// Disable all
	for (int i = 0; i < EPISODE_ITEM_ACTION_SIZE; i++)
		fEpisodeItems[i]->SetEnabled(episode != NULL);
}

void
MainWindow::UpdateToolBar()
{
	bool channelSelected = GetSelectedSubscription() != NULL;
	bool itemSelected = GetSelectedEpisode() != NULL;

	for (int32 i = 0; BView* view = fToolBar->ChildAt(i); i++) {
		BButton* button = dynamic_cast<BButton*>(view);
		if (button == NULL)
			continue;
		BMessage* message = button->Message();
		if (message == NULL)
			continue;
		int16 position;
		if (message->what == PERFORM_ACTION && message->FindInt16("action_position", &position) == B_OK) {
			switch (position) {
				case IACTION_CHANNEL_ADD_REQUEST:
					button->SetEnabled(true);
					break;
				case IACTION_CHANNEL_REMOVE:
				case IACTION_CHANNEL_CHECK:
				case IACTION_CHANNEL_WWW:
					button->SetEnabled(channelSelected);
					break;
				case IACTION_ITEM_PLAY:
				case IACTION_ITEM_DOWNLOAD:
				case IACTION_ITEM_STOP:
				case IACTION_ITEM_REMOVE:
					button->SetEnabled(itemSelected);
					break;
			}
		}
	}
}


void
MainWindow::_ShowDocumentation()
{
	BPathFinder pathFinder;
	BStringList paths;
	BPath path;
	BEntry entry;

	status_t error = pathFinder.FindPaths(B_FIND_PATH_DOCUMENTATION_DIRECTORY,
		"packages/bepodder", paths);

	for (int i = 0; i < paths.CountStrings(); ++i) {
		if (error == B_OK && path.SetTo(paths.StringAt(i)) == B_OK
				&& path.Append(B_TRANSLATE_COMMENT("ReadMe.html",
				"Path to the help file. Only change if a translated file is "
				"provided.")) == B_OK) {
			entry = path.Path();
			entry_ref ref;
			entry.GetRef(&ref);
			be_roster->Launch(&ref);
		}
	}
}
