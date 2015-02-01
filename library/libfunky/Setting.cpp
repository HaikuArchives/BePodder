#include "Setting.h"
#include <StorageKit.h>
#include "Utils.h"

Setting::Setting(const char* settings_name) {

	if(!settings_name) {
		filename.SetTo(GetAppFilename());
		filename << ".conf";
		}
	else	
		filename.SetTo(settings_name);

}


void
Setting::Store(){
	//salvataggio rapido ed indolore dei settaggi!
	
	BPath		xpath;
	status_t	err;
	
	find_directory (B_USER_SETTINGS_DIRECTORY, &xpath, true);
	xpath.Append (filename.String());
	
	BFile	*file=new BFile();
	err=file->SetTo(xpath.Path(),B_WRITE_ONLY|B_ERASE_FILE|B_CREATE_FILE);
	
	if(err != B_OK) {
	
		//printf ("file error %ld\n",err);
		delete file;
		return;
	}
	
	Flatten(file);
	
	delete file;
	
}

void
Setting::Retrive(){

	
	status_t	err;
		
	BFile	file;
	err=file.SetTo(SettingPath().String(),B_READ_ONLY);
	
	if(err == B_OK)  {
	 	MakeEmpty();
		Unflatten(&file);
	}
}

BString
Setting::SettingPath(){
	BPath		xpath;
	find_directory (B_USER_SETTINGS_DIRECTORY, &xpath, true);
	xpath.Append (filename.String());
	return xpath.Path();
}
