#ifndef		_AutoFiletypeMap_H_
#define		_AutoFiletypeMap_H_

#define	MEDIAPLAYER	0
#define	SOUNDPLAY	1
#define	VLC 		2
#define	NONE		3


struct coppia {
	const char*  extension;
	const int	 application;
	const char*	 alert;
};


const coppia autotypes[]= { 
	{".3gp",VLC,_T("alert_3gp")},
	{".3g2",VLC,_T("alert_3gp")},
	{".avi",MEDIAPLAYER,NULL},
	{".flv",VLC,NULL},
	{".m3u",SOUNDPLAY,NULL},
	{".m4a",VLC,NULL},
	{".m4b",VLC,NULL},
	{".m4p",VLC,NULL},
	{".m4v",VLC,NULL},
	{".mov",VLC,NULL},
	{".mp4",VLC,NULL},
	{".swf",NONE,_T("alert_swf")},
	{".wma",VLC,NULL},
	{".wmv",VLC,NULL}
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
