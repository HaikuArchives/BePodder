#include "BPSettingsWindow.h" 
#include "SettingsTextView.h"

#include <ListView.h> 
#include <Message.h>
#include "BPLocale.h"
#include <StorageKit.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <StringView.h>
#include "PBox.h"
#include <Alert.h>
#include "Setting.h"
extern Setting podder_settings;

//#include "LicenceInfo.h"
//extern LicenceInfo		linfo;


BPSettingsWindow::BPSettingsWindow() :
	SettingsWindow(&podder_settings,be_app,APPLY_SETTINGS)
{

	BMessage data;
	
	podder_settings.FindMessage(SETTINGS_FILETYPE,&data);
	BBox*	abox = CreateFileTypeBox(data);
	AddBox(_T("FileType") ,abox);
	data.MakeEmpty();
	
	
	podder_settings.FindMessage(SETTINGS_CHANNELS,&data);
	//if(!linfo.Valid()) data.MakeEmpty();
	abox = CreateChannelsBox(data);
	AddBox(_T("Channels"), abox);
	//if(!linfo.Valid())  { FillOfValue(abox); FixLabel(abox); }
	data.MakeEmpty();
	
	//downloads
	podder_settings.FindMessage(SETTINGS_DOWNLOAD,&data);
	//if(!linfo.Valid()) data.MakeEmpty();
	abox = CreateDownloadBox(data);
	AddBox(_T("Download"),abox);
	//if(!linfo.Valid())  { FillOfValue(abox); FixLabel(abox); }
	data.MakeEmpty();
	
	// Proxy
	podder_settings.FindMessage(SETTINGS_PROXY,&data);
	//if(!linfo.Valid()) data.MakeEmpty();
	abox = CreateProxyBox(data);
	AddBox(_T("Proxy"), abox);
	//if(!linfo.Valid())  { FillOfValue(abox); FixLabel(abox); }
	data.MakeEmpty();
	
	// Updates
	podder_settings.FindMessage(SETTINGS_UPDATES,&data);
	abox = CreateUpdateBox(data);
	AddBox(_T("Updates"), abox);
	data.MakeEmpty();
	
	
	//slist->Select(0);
	Select(0);

}


void	
BPSettingsWindow::FillOfValue(BView* box){ //this name is fake! ;)
	int32 count = box->CountChildren();
	for(int i=0;i<count;i++)
		FillOfValue(box->ChildAt(i));
	
	if(dynamic_cast<BControl*>(box))
		(dynamic_cast<BControl*>(box))->SetEnabled(false);
		
	if(dynamic_cast<BMenuField*>(box))
		(dynamic_cast<BMenuField*>(box))->SetEnabled(false);
}

void	
BPSettingsWindow::FixLabel(BView* box){ //this name is fake! :p
		BView* child = box->FindView("pbox_title");
		
		if(!child)
				return;
		SettingsTextView*	stw=dynamic_cast<SettingsTextView*>(child);
		if(!stw) return;
		
		BString text = stw->Text();
		text << " " << _T("(only in registred version!)");
		
		stw->SetText(text.String());

}					
BBox*
BPSettingsWindow::CreateGeneralBox(BMessage data){
	/*
	BMessage main_msg;
	
	BMessage num_download;
	num_download.AddString("name","starton_workspace");
	num_download.AddString("description", _T("Max Downloads")); 
	num_download.AddInt32("type",B_INT32_TYPE);
	num_download.AddInt32("valid_value",1);
	num_download.AddInt32("valid_value",2);
	num_download.AddInt32("valid_value",3);
	num_download.AddInt32("valid_value",4);
	num_download.AddInt32("valid_value",5);
	num_download.AddInt32("default", 1);
		
	main_msg.AddMessage("setting", &num_download);
	main_msg.AddString("name",_T("Downloads"));
	
	
	PBox *proxy=new PBox(dxrect,main_msg,data,SETTINGS_	GENERAL);
	return (BBox*)proxy;*/
	return NULL;
}

BBox*	
BPSettingsWindow::CreateUpdateBox(BMessage data) {
	BMessage main_msg;
	BMessage setting;
	setting.AddString("name","check_updates");
	setting.AddString("description", _T("Check for updates at startup")); 
	setting.AddInt32("type",B_BOOL_TYPE);
	setting.AddBool("default", true);
	
	main_msg.AddMessage("setting", &setting);
	main_msg.AddString("name",_T("Updates"));
	
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_UPDATES);
	return (BBox*)proxy;
}

BBox*
BPSettingsWindow::CreateDownloadBox(BMessage data){
	
	BMessage main_msg;
	
	BMessage num_download;
	num_download.AddString("name","max_downloads");
	num_download.AddString("description", _T("Max Downloads")); 
	num_download.AddInt32("type",B_INT32_TYPE);
	num_download.AddInt32("valid_value",1);
	num_download.AddInt32("valid_value",2);
	num_download.AddInt32("valid_value",3);
	num_download.AddInt32("valid_value",4);
	num_download.AddInt32("valid_value",5);
	num_download.AddInt32("default", 1);
		
	main_msg.AddMessage("setting", &num_download);
	main_msg.AddString("name",_T("Downloads"));
	
	
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_DOWNLOAD);
	return (BBox*)proxy;
}


BBox*	
BPSettingsWindow::CreateFileTypeBox(BMessage data){
	
	// ATTENZIONE NON CAMBIARE QUESTI SETTATGGI
	// SE NON SI MODIFICANO LE RISPETTIVE CLASSI DI LOAD_SETTIINGS
	
	BMessage setting;
	setting.AddString("name","browser");
	setting.AddString("description", _T("Browser")); 
	setting.AddInt32("type",B_STRING_TYPE);
	setting.AddString("valid_value","Firefox"); //0
	setting.AddString("valid_value","NetPositive"); //1
	setting.AddString("valid_value","System Default"); //2	
	setting.AddString("default", "NetPositive");
	
	BMessage setting2;
	setting2.AddString("name","player");
	setting2.AddString("description", _T("Player")); 
	setting2.AddInt32("type",B_STRING_TYPE);
	setting2.AddString("valid_value","MediaPlayer");
	setting2.AddString("valid_value","SoundPlay");
	setting2.AddString("valid_value","VideoLAN");
	setting2.AddString("valid_value","Auto");
	setting2.AddString("default", "Auto");
	
	BMessage main_msg;	
	main_msg.AddMessage("setting", &setting);
	main_msg.AddMessage("setting", &setting2);
	
	main_msg.AddString("name",_T("FileType"));
	
		
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_FILETYPE);
	return (BBox*)proxy;
}

BBox*	
BPSettingsWindow::CreateChannelsBox(BMessage data){
	
	BMessage setting;
	setting.AddString("name","check_startup");
	setting.AddString("description", _T("Check all channels on startup")); 
	setting.AddInt32("type",B_BOOL_TYPE);
	
	BMessage setting2;
	setting2.AddString("name","check_time");
	setting2.AddString("description", _T("Check the channel every")); 
	setting2.AddInt32("type",B_STRING_TYPE);
	setting2.AddString("valid_value",_T("never"));
	setting2.AddString("valid_value",_T("5 minutes"));
	setting2.AddString("valid_value",_T("15 minutes"));
	setting2.AddString("valid_value",_T("30 minutes"));
	setting2.AddString("valid_value",_T("1 hour"));
	setting2.AddString("valid_value",_T("2 hour"));
	setting2.AddString("default", _T("never"));
			
	BMessage main_msg;	
	main_msg.AddMessage("setting", &setting);
	main_msg.AddMessage("setting", &setting2);
	main_msg.AddString("name",_T("Channels"));
	
		
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_CHANNELS);
	return (BBox*)proxy;
}
 					
BBox*	
BPSettingsWindow::CreateProxyBox(BMessage data){

	BMessage main_msg;
	
	BMessage enable_proxy;
	enable_proxy.AddString("name","enable");
	enable_proxy.AddString("description", _T("Enable Proxy")); 
	enable_proxy.AddInt32("type",B_BOOL_TYPE);
	enable_proxy.AddString("enable_control","address");
	enable_proxy.AddString("enable_control","port");
	enable_proxy.AddString("enable_control","username");
	enable_proxy.AddString("enable_control","password");
	
	BMessage address;
	address.AddString("name","address");
	address.AddString("description", _T("Address")); 
	address.AddInt32("type",B_STRING_TYPE);
	
	BMessage port;
	port.AddString("name","port");
	port.AddString("description", _T("Port"));
	port.AddInt32("type",B_INT32_TYPE);
		
	BMessage res;
	res.AddString("name","username");
	res.AddString("description", _T("Username"));
	res.AddInt32("type",B_STRING_TYPE);
	
	BMessage year;
	year.AddString("name","password");
	year.AddString("description", _T("Password"));
	year.AddInt32("type",B_STRING_TYPE);
	year.AddBool("is_secret",true);
	
	main_msg.AddMessage("setting", &enable_proxy);
	main_msg.AddMessage("setting", &address);
	main_msg.AddMessage("setting", &port);
	main_msg.AddMessage("setting", &res);
	main_msg.AddMessage("setting", &year);
	main_msg.AddString("name",_T("Proxy"));
			
	
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_PROXY);
	
	return (BBox*)proxy;
}

