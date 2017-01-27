#include "MainWindow.h"
#include "Application.h"
#include "BPSettingsWindow.h"
#include "AddWindow.h"
#include "DirectoryWindow.h"
#include "HelpWindow.h"

#include <MenuBar.h>
#include <MenuItem.h>
#include <Alert.h>
#include <Button.h>
#include <View.h>
#include <Box.h>
#include <ScrollBar.h>
#include <FilePanel.h>
#include "BPLocale.h"
#include <StorageKit.h>

#include "ColumnListView.h"
#include "ColumnTypes.h"

#include "EpisodeListItem.h"
#include "SubscriptionListItem.h"

#include "SplitPane.h"
#include "Utils.h"

#include "IActionMenuItem.h"

#include "MemoryArchive.h"
#include "htmlparse.h"
#include "ImageButton.h"
#include <Bitmap.h>
#include "DownloadManager.h"
 
#include "IconTextRender.h" 
#include "TitleTextRender.h"
#include "GreenBoxTextRender.h"
//#include "BitmapTextRender.h"

#include "curl/curl.h"
#include <parsedate.h>

//#include "runview/RunView.h" 
#include "ItemRunView.h"
#include "runview/Theme.h"

#include "RVActionBP.h"
#include "RVActionBPOpenURL.h" 

#include "SectionSelector.h"

#include "DownloadListView.h"
#include "DownloadListItem.h"
#include "MainController.h"
#include "IActionManagerBP.h"
#include "GroupItem.h"

#include "IABPGroup.h"
#include "OPMLParser.h"

#ifdef ZETA
#include <sys_apps/Tracker/Icons.h>
#endif

//messages:
	#define		DOWNLOAD_ITEM				'invD'
	#define		DOWNLOAD_CHANNEL			'addu'


	#define 	ITEM_SELECTED					'selD'
	#define 	CHANGE_WORKSPACE			'wsch'
	#define		SET_FULLSCREEN				'full'
	#define 	SHOW_SETTINGS				'setp'
	#define		SHOW_TOOLBAR					'notb'
	#define		SECTION_SELECTED			'sese'
	
	#define		SHOW_HELP			'help'
	#define		WEB_PAGE				'webp'
	
	#define		SHOW_DIRECTORY		'dish'
	
	#define		IMPORT_OPML		'imop'
	#define		EXPORT_OPML		'exop'
	#define 	PARSE_OPML	'paop'

// file size:
	const int64 	kKB_SIZE				= 1024;
	const int64 	kMB_SIZE				= 1048576;
	const int64 	kGB_SIZE				= 1073741824;
	const int64 	kTB_SIZE				= kGB_SIZE * kKB_SIZE;

//misc UI:
	#define		CHANNELS_BAR_X	7 
	#define		ITEMS_BAR_X			280
	#define		BAR_Y					12


// Singletons:

//#include "LicenceInfo.h"
//extern LicenceInfo		linfo;

extern IActionManagerBP		action_manager;


MainWindow::MainWindow()
: BWindow(BRect(100,100,970,607), "BePodder", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS){
	
	#ifdef ZETA
	 	AddIcon("folder",new BBitmap(GetTrackerIcon("application/x-vnd.Be-directory",16)));
	 	AddIcon("image",new BBitmap(GetTrackerIcon("image/png",16)));
	#else
		BBitmap* folder=new BBitmap(BRect(0,0,15,15), 0, B_RGBA32);
		BBitmap* image=new BBitmap(BRect(0,0,15,15), 0, B_RGBA32);
		
		BMimeType type("application/x-vnd.Be-directory");
		type.GetIcon(folder,B_MINI_ICON);
		
		BMimeType type2("image");
		type2.GetIcon(image,B_MINI_ICON);
		
		AddIcon("folder",folder);
		AddIcon("image",image);
		
	#endif
}

void
MainWindow::init(MainController* controller){	

	
	SetController(controller);


    rgb_color bgcolor = ui_color(B_PANEL_BACKGROUND_COLOR);
#ifdef ZETA	
	SetWindowColor(bgcolor);
#endif	
	
	BRect rect(Bounds());
	rect.top = CreateMenuBar() + 1;
	
	view=new BBox(rect,NULL, B_FOLLOW_ALL, B_WILL_DRAW,B_NO_BORDER);
//	view->SetViewColor(bgcolor);
	AddChild(view);


	
	const BFont *font = be_plain_font;
	rect = BRect(0,0,47,31);
	
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
					
	channelView=new BView(BRect(CHANNELS_BAR_X,BAR_Y,CHANNELS_BAR_X+47+66*(COUNT-1),BAR_Y+31),"",B_FOLLOW_NONE,B_WILL_DRAW);
	channelView->SetViewColor(bgcolor);
	view->AddChild(channelView);
			
					
	//automatic for the people
	int i;
	for(i=0;i<COUNT;i++){
		ImageButton *tasto1= new ImageButton(rect,"",action_manager.GetAction(ChannelBar[i]));
		channelView->AddChild(tasto1);
		rect.OffsetBy(66,0);
		
		fChannelMenu->AddItem(new IActionMenuItem(action_manager.GetAction(ChannelBar[i])));
		channels->AddItem(new IActionMenuItem(action_manager.GetAction(ChannelBar[i])));
	}	
	
	//extras :)
	fChannelMenu->AddSeparatorItem();
	fChannelMenu->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_ENCLOSURE_FOLDER)));
	fChannelMenu->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_CHECK_ALL)));	
	fChannelMenu->AddSeparatorItem();
	fChannelMenu->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_SHOW_IMAGE)));
	fChannelMenu->SetTargetForItems(be_app);
	
	channels->AddSeparatorItem();
	channels->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_ENCLOSURE_FOLDER)));
	channels->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_CHECK_ALL)));	
	channels->AddSeparatorItem();
	channels->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_SHOW_IMAGE)));
	channels->SetTargetForItems(be_app);
	
	rect = BRect(0,0,47,31);
	rect.OffsetBy(338,0);
	
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
					
	itemsView=new BView(BRect(ITEMS_BAR_X,BAR_Y,ITEMS_BAR_X+382+66*(COUNT2-1),BAR_Y+31),"",B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW);  
	itemsView->SetViewColor(bgcolor);
	//itemsView->SetDoubleBuffering(true);
	view->AddChild(itemsView);				
					
	//automatic for the people
	for(i=0;i<COUNT2;i++){
		ImageButton *tasto1= new ImageButton(rect,"",action_manager.GetAction(ItemBar[i]));
		itemsView->AddChild(tasto1);
		rect.OffsetBy(66,0);
		
		
		fItemMenu->AddItem(new IActionMenuItem(action_manager.GetAction(ItemBar[i])));
		items->AddItem(new IActionMenuItem(action_manager.GetAction(ItemBar[i])));
	}	
	
	//extras :)
	fItemMenu->AddSeparatorItem();
	fItemMenu->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_ITEM_WWW)));
	fItemMenu->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_ENCLOSURE_FOLDER)));
	
	items->AddSeparatorItem();
	items->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_ITEM_WWW)));
	items->AddItem(new IActionMenuItem(action_manager.GetAction(IACTION_CHANNEL_ENCLOSURE_FOLDER)));
		
	fItemMenu->SetTargetForItems(be_app);
	items->SetTargetForItems(be_app);
	
	// GROUPS MENU
	
	fGroupMenu = new BPopUpMenu("GroupsMenu");
	fGroupMenu->SetFont(font);
	const	 int COUNT4 = 4;
	const int groupBar[COUNT4] = 
					{
						IACTION_GROUP_CHECK,
						IACTION_GROUP_ADD,
						IACTION_GROUP_RENAME,
						IACTION_GROUP_REMOVE,
					};
					
					
	//automatic for the people
	for(i=0;i<COUNT4;i++){
		fGroupMenu->AddItem(new IActionMenuItem(action_manager.GetAction(groupBar[i])));
		groups->AddItem(new IActionMenuItem(action_manager.GetAction(groupBar[i])));
	}	
	
	groups->SetTargetForItems(be_app);
	fGroupMenu->SetTargetForItems(be_app);
	
	//DOWNLOAD MENU
	
	
	fDownloadMenu = new BPopUpMenu("DownMenu");
	fDownloadMenu->SetFont(font);
	
	const	 int COUNT3 = 5;
	const int downBar[COUNT3] = 
					{
						IACTION_DOWNLOAD_PLAY,
						IACTION_DOWNLOAD_STOP,	
						IACTION_DOWNLOAD_DOWNLOAD,
						IACTION_DOWNLOAD_FIND_ITEM,
						IACTION_DOWNLOAD_REMOVE
					};
					
					
	//automatic for the people
	for(i=0;i<COUNT3;i++){
		fDownloadMenu->AddItem(new IActionMenuItem(action_manager.GetAction(downBar[i])));
	}	
	
	fDownloadMenu->SetTargetForItems(be_app);
	
	
	//BButton *tasto8= new BButton(BRect(725,11,835,41),"Send","Please feedback :)",new BMessage('send'));
	//view->AddChild(tasto8);
	//tasto8->SetToolTipText(_T("Submit bug or idea"));
	
	
	//--------------------------------------------------------451
	
	//fStack = new StackedView(BRect(451,52,840,300),"name",B_FOLLOW_ALL, B_WILL_DRAW|B_NAVIGABLE);
	
	theStack = new EpisodeListView(BRect(451,52,840,300));
	
	BMessage* msg=new BMessage(DOWNLOAD_ITEM);
	theStack->SetInvocationMessage(msg);
	
	msg=new BMessage(ITEM_SELECTED);
	msg->AddInt32("buttons",0);
	theStack->SetSelectionMessage(msg);
	
		
	sx_list=new SubscriptionListView(BRect(0,0,210,377));
	

	
	
	
	
	fSelector=new SectionSelector(BRect(0,0,100,100),new BMessage(SECTION_SELECTED));
	fSelector->SetTarget(this);
	
	BRect k = Bounds();
		
	dxsplit =new SplitPane(BRect(451,52,860,477),theStack,fSelector,B_FOLLOW_ALL);
	dxsplit->SetViewInsetBy(BPoint(0,0));
	dxsplit->SetAlignment(B_HORIZONTAL);
	dxsplit->SetViewColor(bgcolor);
	dxsplit->SetBarPosition(BPoint(k.Width()/2.0f-170, k.Height()/2.0f+38));
	dxsplit->SetMinSizeOne(BPoint(0,84));
	dxsplit->SetMinSizeTwo(BPoint(0,161));
		
	split =new SplitPane(BRect(10,52,860,477),sx_list,dxsplit,B_FOLLOW_ALL);
	split->SetViewInsetBy(BPoint(0,0));
	split->SetViewColor(bgcolor);
	split->SetBarPosition(BPoint(k.Width()/2.0f-170, k.Height()/2.0f));
	split->SetMinSizeOne(BPoint(200,0));
	split->SetMinSizeTwo(BPoint(200,0));
	
	view->AddChild(split);
	
	BView*	fakeView=new BView(fSelector->Bounds(),"fakeView",B_FOLLOW_ALL,B_WILL_DRAW);
	fakeView->AddChild(down_list=new DownloadListView(fakeView->Bounds()));
	 
	fSelector->AddSection("info0.png",CreateItemInfoView(), _T("Show episode info") );
	fSelector->AddSection("info1.png",CreateChannelInfoView(), _T("Show subscription info"));
	fSelector->AddSection("info2.png",fakeView,  _T("Show downloads info") );
	fSelector->Select(0);
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
	
	BRect scrollRect(0,0,100,100 );

	fItemText = new ItemRunView(scrollRect, "text", fTheme,B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE);

#ifdef ZETA
	fItemText->SetViewUIColor(B_UI_DOCUMENT_BACKGROUND_COLOR);
	fItemText->SetLowUIColor(B_UI_DOCUMENT_BACKGROUND_COLOR);
	fItemText->SetHighUIColor(B_UI_DOCUMENT_TEXT_COLOR);
#endif
	
	BScrollView *fTextScroll = new BScrollView("scroller", fItemText,B_FOLLOW_ALL_SIDES, 0,false, true, B_PLAIN_BORDER);
	
	
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
#ifdef ZETA
	fChannelText->SetViewUIColor(B_UI_DOCUMENT_BACKGROUND_COLOR);
	fChannelText->SetLowUIColor(B_UI_DOCUMENT_BACKGROUND_COLOR);
	fChannelText->SetHighUIColor(B_UI_DOCUMENT_TEXT_COLOR);
#endif	
	BScrollView *fTextScroll = new BScrollView("scroller2", fChannelText,B_FOLLOW_ALL_SIDES, 0,false, true, B_PLAIN_BORDER);
		
	fChannelText->AddAction("bepodder",new RVActionBP());
	fChannelText->SetDefaultOpenURLAction(new RVActionBPOpenURL(fController));
	
	return fTextScroll;
	

}

float
MainWindow::CreateMenuBar(){

	BMenuBar *poddermenubar = new BMenuBar(BRect(0,0,1,1),"Poddermenubar"); //,B_FOLLOW_RIGHT | B_FOLLOW_TOP,B_ITEMS_IN_ROW,true);

	
	BMenu  *podderfile = new BMenu(_T("File"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(podderfile);
	
	BMenuItem* importOmpl = new BMenuItem(_T("Import OPML...") , new BMessage(IMPORT_OPML), 0, 0);
	podderfile->AddItem(importOmpl);

	
	BMenuItem *podderaboutitem = new BMenuItem(_T("About..."),new BMessage(B_ABOUT_REQUESTED),0,0);
	podderaboutitem->SetTarget(be_app);
	podderfile->AddItem(podderaboutitem);
	
	BMenuItem *quititem = new BMenuItem(_T("Quit"),new BMessage(B_QUIT_REQUESTED),'Q',0);
	quititem->SetTarget(be_app);
	podderfile->AddItem(quititem);
	
	BMenu  *settingsfile = new BMenu(_T("Settings"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(settingsfile);
	
	BMenuItem *podderpreferencesitem = new BMenuItem(_T("Preferences..."),new BMessage(SHOW_SETTINGS),'P',0);
	settingsfile->AddItem(podderpreferencesitem);
	
	groups = new BMenu(_T("Groups"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(groups);
	
	channels = new BMenu(_T("Subscriptions"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(channels);
	
	items = new BMenu(_T("Episodes"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(items);	
	
	BMenu  *directoryfile = new BMenu(_T("Lists"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(directoryfile);
	
	
	AddDirectoryItem(directoryfile,_T("Podcast list"),"http://www.funkyideasoft.com/directories/bepodder_podcasts.opml.tar.gz",'L');
	AddDirectoryItem(directoryfile,_T("Videoblog list"),"http://www.funkyideasoft.com/directories/bepodder_videoblog.opml.tar.gz",'M');
	AddDirectoryItem(directoryfile,_T("Imagefeeds list"),"http://www.funkyideasoft.com/directories/bepodder_imagefeeds.opml.tar.gz",'I');
	AddDirectoryItem(directoryfile,_T("Newsfeeds list"),"http://www.funkyideasoft.com/directories/bepodder_newsfeeds.opml.tar.gz",'N');
	AddDirectoryItem(directoryfile,_T("BitTorrentfeeds list"),"http://www.funkyideasoft.com/directories/bepodder_bittorrentfeeds.opml.tar.gz",'B');
	AddDirectoryItem(directoryfile,_T("Internationalfeeds list"),"http://www.funkyideasoft.com/directories/bepodder_internationalfeeds.opml.tar.gz",'J');
	
	BMenu  *podderview = new BMenu(_T("View"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(podderview);
	
	fullscreenitem = new BMenuItem(_T("Full screen"),new BMessage(SET_FULLSCREEN),'F',0);
	podderview->AddItem(fullscreenitem);
	fullscreenitem->SetMarked(false);
	
	notoolbar= new BMenuItem(_T("Toolbar"),new BMessage(SHOW_TOOLBAR),'T',0);
	podderview->AddItem(notoolbar);
	notoolbar->SetMarked(true);
	
	
	ChannelSize = new BMenu(_T("Subscriptions view"));
	podderview->AddItem(ChannelSize);
	ChannelSize->SetRadioMode(true);
	
	BMenuItem* temp;
	
	temp= new BMenuItem(_T("Big items"),new BMessage('bigg'));
	ChannelSize->AddItem(temp);
	temp->SetMarked(true);
	
	temp=new BMenuItem(_T("Small items"),new BMessage('smal'));
	ChannelSize->AddItem(temp);
	temp->SetMarked(false);
	
	ChannelSize->SetEnabled(true); //linfo.Valid());	
		
	//--------------------------------------------------------------------------------------------------------------
	
	
	setworkspace= new BMenu(_T("Move BePodder onto workspace:"));
	podderview->AddItem(setworkspace);
	
	for(int i=0;i<count_workspaces();i++){
		BMenuItem* workspace1;
		BString text; 
		text << i + 1;
		setworkspace->AddItem(workspace1 = new BMenuItem(text.String(), new BMessage(CHANGE_WORKSPACE), 0));
		if(i ==	current_workspace()  )
			workspace1->SetMarked(true);
	}
	
				
	BMenu  *helpfile = new BMenu(_T("Help"),B_ITEMS_IN_COLUMN);
	poddermenubar->AddItem(helpfile);
	
	
	BMessage *msg=new BMessage(SHOW_HELP);
	BMenuItem *helpitem1 = new BMenuItem(_T("Getting started"),msg,'G',0);
	helpfile->AddItem(helpitem1);
	
	msg=new BMessage(SHOW_HELP);
	msg->AddString("page","shortcuts");
	BMenuItem *shortcutitem = new BMenuItem(_T("Keyboard shortcuts"),msg,'K',0);
	helpfile->AddItem(shortcutitem);
	
	BMessage *webmsg=new BMessage(WEB_PAGE);
	//full url (introduction.html)
	BString where(GetAppRelativePath());
	where << "/tutorial/introduction.html";
	where.Prepend("file://");
	
	webmsg->AddString("url",where.String());
	helpfile->AddItem( new BMenuItem(_T("Tutorials"),webmsg,0,0));
	
	
	helpfile->AddSeparatorItem();
	
	webmsg=new BMessage(WEB_PAGE);
	webmsg->AddString("url","http://www.funkyideasoft.com/support.html");
	helpfile->AddItem( new BMenuItem(_T("Online support"),webmsg,0,0));
	
	helpfile->AddSeparatorItem();
	BMenuItem* updates;
	helpfile->AddItem(updates=new BMenuItem(_T("Check for updates"),new BMessage('ckup'),0,0));
	updates->SetTarget(be_app);
	
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
		
		int32 size;
		//default Subscription Items size:
		if(data->FindInt32("channel_size",&size) == B_OK){
			sx_list->SetItemSize((SubscriptionSize)size);
			ChannelSize->ItemAt(size)->SetMarked(true);
		}    	
		
		ChannelSize->SetEnabled(true);//linfo.Valid());	
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
			
			BPAlert *alert = new BPAlert("BePodder",_T("Loading help..."),NULL,NULL,NULL,B_WIDTH_AS_USUAL, LoadIcon("enqueued-32.png"));
			alert->Go(NULL);
			
			HelpWindow *help1 = new HelpWindow();
			help1->Show();
			
			BString page;
			if(msg->FindString("page",&page)==B_OK)
				help1->SetPage(page);
			
			alert->PostMessage(B_QUIT_REQUESTED);
		} 
		break;
		
		case SET_FULLSCREEN:
			SetFullscreen(!fullscreenitem->IsMarked());
		break;
		
		case CHANGE_WORKSPACE:
		{
			 msg->PrintToStream();
			uint32 index=(uint32)msg->FindInt32("index");
					
    		uint32 space = 0x1UL; 
    		space <<= index;
		   
			SetWorkspaces(space);
			if( setworkspace->FindMarked() ) setworkspace->FindMarked()->SetMarked(false);
			if(setworkspace->ItemAt(index)) setworkspace->ItemAt(index)->SetMarked(true);
		} 
		break;
				
		case CHECK_ALL:
			fController->CheckAllChannel();
		break;
		
		case ARCHIVE_PARSE:
		{
			//show the alert
			BPAlert *alert = new BPAlert("BePodder",_TT("alert3"),NULL,NULL,NULL,B_WIDTH_AS_USUAL, LoadIcon("enqueued-32.png"));
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
				elv->GetScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = elv->GetScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fItemMenu->FindMarked())
					fItemMenu->FindMarked()->SetMarked(false);
				
				elv->SelectionMessage()->ReplaceInt32("buttons",0);
				fItemMenu->Go(p2, true, true, true);
			
			}
		}
		break;
		
		case DOWNLOAD_SELECTED:
		{
						
			if(msg->FindInt32("buttons")==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				down_list->GetScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = down_list->GetScrollView()->ConvertToScreen(point);
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
			BFilePanel *filePanel	= new	BFilePanel(B_OPEN_PANEL);
			filePanel->SetMessage(new BMessage(PARSE_OPML));
			filePanel->SetTarget(BMessenger(this));
			filePanel->Show();
			//try to parse
			
			//foreach add to the window
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

				
//			BPAlert *alert = new BPAlert("BePodder",,_T("Error"),_T("Close"),NULL,B_WIDTH_AS_USUAL, LoadIcon("logo-64.png"));
//			if(alert->Go()==0)
//				fController->OpenURL("http://www.funkyideasoft.com/downloads.html");
			}			
			msg->PrintToStream();
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
				sx_list->GetScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = sx_list->GetScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fChannelMenu->FindMarked())
					fChannelMenu->FindMarked()->SetMarked(false);
				
				sx_list->SelectionMessage()->ReplaceInt32("buttons",0);
				fChannelMenu->Go(p2, true, true, true);
		}
}

void	
MainWindow::SelectGroup(GroupItem* grp, int32 bottons){
			
			if(!grp) return;
			
			fController->SwitchChannel(NULL);
			
			if(bottons==B_SECONDARY_MOUSE_BUTTON){
				
				BPoint point;
				uint32 state;
				sx_list->GetScrollView()->GetMouse(&point,&state);
				
				BPoint p2 = sx_list->GetScrollView()->ConvertToScreen(point);
				p2.x -= 5.0;
				p2.y -= 5.0;
			
				
				if(fGroupMenu->FindMarked())
					fGroupMenu->FindMarked()->SetMarked(false);
				
				sx_list->SelectionMessage()->ReplaceInt32("buttons",0);
				fGroupMenu->Go(p2, true, true, true);
		}
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
	*outRect= sx_list->GetScrollView()->ConvertToScreen(rect);
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
	
	
	switch(fSelector->Selected()){
		case 0:
			ShowItemDescription(archive);
		break;
		case 1:
			ShowChannelDescription(archive);
		break;
		
		default:break;
	};
		
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

		char	dateString[256];
		tm		time_data;
		time_t datetime=*curtime;
		localtime_r(&datetime, &time_data);
		
		//"%A, %B %d %Y, %I:%M:%S %p"		
		strftime(dateString, 256, "%a, %B %d %Y, %I:%M:%S %p", &time_data);
		
		fItemText->Append("   - ",C_TEXT,C_ACTION,F_TIMESTAMP);
		fItemText->Append(dateString,C_TEXT,C_ACTION,F_TIMESTAMP);
	} 
	else
		fItemText->Append(" ",C_TEXT,C_ACTION,F_TIMESTAMP);
	
	fItemText->Append("\n",C_TEXT,C_TEXT,F_TEXT);
	
	if(archive->GetDataString(ITEM_TITLE) !="")	
		fItemText->Append(archive->GetDataString(ITEM_TITLE).String(),C_TEXT,C_TEXT,F_ACTION);
	
	if(archive->GetDataString(ITEM_LINK) != ""){
		fItemText->AppendURL("  [www]",archive->GetDataString(ITEM_LINK) .String(),F_TEXT);
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
		fItemText->AppendURL(_T("Enclosure: "),action.String(),C_ACTION,F_TEXT);
				
		BString local(archive->GetDataString(ITEM_ENCLOSURE_LOCALPATH));
		//path.Append(local.String());
		//folder_url.SetTo(path.Path());
		//fItemText->AppendURL(local.String(),folder_url.String(),C_ACTION,F_TEXT);
		
		BString bottom;
		bottom << local;
		
		BString filetype(archive->GetDataString(ITEM_ENCLOSURE_TYPE) );
		if( filetype !="")
			bottom << " (" << filetype<< ") " ;
		
		
		off_t* len=0;
		if( archive->GetData(ITEM_ENCLOSURE_LENGTH,(const void**)&len)>0 && len>0 ){		 
				off_t		size=*len;
				char		str[256];
				if (size < kKB_SIZE)
				{
					sprintf(str, "%Ld bytes", size);
				}
				else
				{
					const char*	suffix;
					float 		float_value;
					if (size >= kTB_SIZE)
					{
						suffix = "TiB";
						float_value = (float)size / kTB_SIZE;
					}
					else if (size >= kGB_SIZE)
					{
						suffix = "GiB";
						float_value = (float)size / kGB_SIZE;
					}
					else if (size >= kMB_SIZE)
					{
						suffix = "MiB";
						float_value = (float)size / kMB_SIZE;
					}
					else
					{
						suffix = "KiB";
						float_value = (float)size / kKB_SIZE;
					}
					
					sprintf(str, "%.2f %s", float_value, suffix);
					// strip off an insignificant zero so we don't get readings
					// such as 1.00
					char *period = 0;
					char *tmp (NULL);
					for (tmp = str; *tmp; tmp++)
					{
						if (*tmp == '.')
							period = tmp;
					}
					if (period && period[1] && period[2] == '0')
						// move the rest of the string over the insignificant zero
						for (tmp = &period[2]; *tmp; tmp++)
							*tmp = tmp[1];
			}
			bottom << "  [" << str << "]" ;
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
	
	if(archive->GetDataString(CHANNEL_TITLE) != "")
		fChannelText->Append(archive->GetDataString(CHANNEL_TITLE).String(),C_TEXT,C_TEXT,F_ACTION);

	if(archive->GetDataString(CHANNEL_WEBLINK) != ""){
		fChannelText->AppendURL("  [www]",archive->GetDataString(CHANNEL_WEBLINK) .String(),F_TEXT);
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
	
	size = archive->GetData(CHANNEL_IMAGE_DATA,(const void**)&buffer);
	
	if( size>0){
		BString action("action://bepodder(");
		action << (int)IACTION_CHANNEL_SHOW_IMAGE << ")";
		fChannelText->Append("\n\n",C_TEXT,C_TEXT,F_TEXT);	
		fChannelText->AppendURL(_T("Show channel image"),action.String(),C_TEXT,F_TEXT);	
		//free(buffer);
	}
	
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
			
				if(!itemsView->IsHidden())
					itemsView->Hide();
				if(!channelView->IsHidden())
					channelView->Hide();
			
			}	else {

				
				notoolbar->SetMarked(true);
				view->ScrollTo(BPoint(0,0));
				split->ResizeBy(point.x,-point.y);
				dxsplit->SetBarPosition(dxsplit->GetBarPosition()-=point);

				if(itemsView->IsHidden())
					itemsView->Show();
				if(channelView->IsHidden())
					channelView->Show();
			}
			
		
}

