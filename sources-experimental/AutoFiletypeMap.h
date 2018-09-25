#ifndef		_AutoFiletypeMap_H_
#define		_AutoFiletypeMap_H_

#include <Catalog.h>

#define	MEDIAPLAYER	0
#define	SOUNDPLAY	1
#define	VLC 		2
#define	NONE		3

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "AutoFileTypeMap"

struct coppia {
	const char*  extension;
	const int	 application;
	const char*	 alert;
};




const coppia autotypes[]= { 
	{".3gp",MEDIAPLAYER,NULL},
	{".3g2",MEDIAPLAYER,NULL},
	{".avi",MEDIAPLAYER,NULL},
	{".flv",MEDIAPLAYER,NULL},
	{".m3u",MEDIAPLAYER,NULL},
	{".m4a",MEDIAPLAYER,NULL},
	{".m4b",MEDIAPLAYER,NULL},
	{".m4p",MEDIAPLAYER,NULL},
	{".m4v",MEDIAPLAYER,NULL},
	{".mov",MEDIAPLAYER,NULL},
	{".mp4",MEDIAPLAYER,NULL},
	{".swf",NONE,B_TRANSLATE("SWF è un formato proprietario per la grafica vettoriale realizzato con il software Flash della Adobe (precedentemente della Macromedia).")},
	{".wma",MEDIAPLAYER,NULL},
	{".wmv",MEDIAPLAYER,NULL},
	{".mp3",MEDIAPLAYER,NULL}
};

int32
FindBestApplication(entry_ref* ref,BString &alert){
	
	int count = sizeof(autotypes)/sizeof(coppia);

	for(int i=0;i<count;i++){
		BString name(ref->name);
		int32 last=name.IFindLast(autotypes[i].extension); 
		if(last>0) {
			alert.SetTo(autotypes[i].alert);
			return autotypes[i].application;
		}
	}
	return -1;
}

#endif
