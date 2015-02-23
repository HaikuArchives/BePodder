#include "ChannelSettingsWindow.h" 
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



ChannelSettingsWindow::ChannelSettingsWindow(BMessage* s,BHandler* h) :
	SettingsWindow(s,h,APPLY_CHANNEL_SETTINGS) {
		
	AddBox(_T("Remove") ,CreateBox(*s));
	Select(0);
}


				

BBox*	
ChannelSettingsWindow::CreateBox(BMessage data) {
	BMessage main_msg;
	
	BMessage when;
	when.AddString("name","when_delete");
	when.AddString("description", _T("WHEN?")); 
	when.AddInt32("type",B_STRING_TYPE);
	when.AddString("valid_value","never"); 			//0
	when.AddString("valid_value","After a check"); 			//1
	when.AddString("valid_value","When there are new episodes"); 	//2
	when.AddString("valid_value","System default"); //3
	when.AddString("default", "never");
	
	main_msg.AddMessage("setting", &when);
	
	
	BMessage what_new;
	what_new.AddString("name","what_delete_new");
	what_new.AddString("description", _T("Episodes nuovi")); 
	what_new.AddInt32("type",B_BOOL_TYPE);
	what_new.AddBool("default", false);
	main_msg.AddMessage("setting", &what_new);
	
	
	what_new.MakeEmpty();
	what_new.AddString("name","what_delete_read");
	what_new.AddString("description", _T("Episodes letti (o con errore)")); 
	what_new.AddInt32("type",B_BOOL_TYPE);
	what_new.AddBool("default", false);	
	main_msg.AddMessage("setting", &what_new);
	
	what_new.MakeEmpty();
	what_new.AddString("name","what_delete_downloaded");
	what_new.AddString("description", _T("Episodes scaricati")); 
	what_new.AddInt32("type",B_BOOL_TYPE);
	what_new.AddBool("default", false);	
	main_msg.AddMessage("setting", &what_new);
	
	
	BMessage who;
	who.AddString("name","who_delete");
	who.AddString("description", _T("WHO?")); 
	who.AddInt32("type",B_STRING_TYPE);
	who.AddString("valid_value","Tutti gli episodes presenti"); 		//0
	who.AddString("valid_value","Episodes piu vecchi di 1 giorno"); 	//1
	who.AddString("valid_value","Episodes piu vecchi di 3 giorni"); 	//2
	who.AddString("valid_value","Episodes piu vecchi di 1 settimana"); 	//3
	who.AddString("valid_value","Episodes piu vecchi di 1 mese"); 		//4
	who.AddString("default", "Tutti gli episodes presenti");
	
	main_msg.AddMessage("setting", &who);	
	
	main_msg.AddString("name",_T("Remove"));
	PBox *proxy=new PBox(BRect(0,0,0,0),main_msg,data,SETTINGS_CHANNEL);
	return (BBox*)proxy;
}

