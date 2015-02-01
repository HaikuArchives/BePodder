#include "R5Locale.h"
#include "BePodder_R5.h"
#include "Message.h"

#define ADD(KEY,VALUE) language->AddString(KEY,VALUE);
 
BMessage *language=NULL;

void
init(){
	language=new BMessage();
	for(int i=0;i<NUMBER;i++)
		ADD(r5languge[i].key,BString(r5languge[i].data));
}

const char*
findKey(const char* key){
	//debugger(key);
	if(!language) init();
	const char* data;
	if(language->FindString(key,&data)==B_OK){
		return data;
	}
		
	return key;
}