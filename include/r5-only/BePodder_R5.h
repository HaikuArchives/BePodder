struct pair {
	const char*	key;
	const char*	data;
};

#define NUMBER		5

const pair r5languge[NUMBER]={
	{"alert1","\nYou are going to remove the selected item.\n\nAre you sure?\n(you will lost the enclosure file if any)"},
	{"alert2a","\nYou are going to remove the channel:\n\n"},
	{"alert2b","\n\nWould you like just to archive this channel (keeping all the files and enclosures downloaded so far) or do you want to DELETE the whole channel from you computer?"},
	{"alert3","\nParsing archive.."},
	{"not down","not downloaded"}	
};