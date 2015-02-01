#include "Logger.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <Directory.h>
#include <Node.h>
#include <Entry.h>
#include <string.h>
#include <stdlib.h>
#include <Path.h>
#include <FindDirectory.h>

#define LogName	"bepodder.log"

log_importance g_verbosity_level = liQuiet;

bool _has_checked_for_tty = false;

void
check_for_tty()
{
	if ( _has_checked_for_tty )
		return;

	_has_checked_for_tty = true;

	if ( !isatty(STDOUT_FILENO) )
	{ // redirect output to ~/im_kit.log if not run from Terminal
		BPath path;

		if (find_directory(B_USER_DIRECTORY,&path,true,NULL) != B_OK)
			// This should never fail..
			return;

		path.Append(LogName);

		close(STDOUT_FILENO);
		open( path.Path(), O_WRONLY|O_CREAT|O_APPEND);
		chmod( path.Path(), 0x600 );
	}
}

bool _has_loaded_log_level = false;

void
load_log_level()
{
	/*
	if ( _has_loaded_log_level )
		return;

	_has_loaded_log_level = true;

	BMessage settings;
	if ( im_load_client_settings("im_server", &settings) != B_OK )
	{
		printf("load_log_level: im_server settings not found\n");
		return;
	}

	const char * level=NULL;
	if ( settings.FindString("log_level", &level) == B_OK )
	{
		// check for the various levels..
		if ( strcmp(level, "Debug") == 0 ) {
			g_verbosity_level = liDebug;
		} else
		if ( strcmp(level, "Low") == 0 ) {
			g_verbosity_level = liLow;
		} else
		if ( strcmp(level, "Medium") == 0 ) {
			g_verbosity_level = liMedium;
		} else
		if ( strcmp(level, "High") == 0 ) {
			g_verbosity_level = liHigh;
		} else
		if ( strcmp(level, "Quiet") == 0 ) {
			g_verbosity_level = liQuiet;
		} else
			g_verbosity_level = liHigh;
	}
	*/
}

const char * gLogLevelText[] = {
	"debug",
	"low",
	"medium",
	"high"
};

// Note: if you change something in this LOG,
// make sure to change the LOG below as the code
// unfortunately isn't shared. :/
void LOG(const char * module, log_importance level, const char *message, const BMessage *msg, ...) {
	va_list varg;
	va_start(varg, msg);
	char buffer[2048];
	vsprintf(buffer, message, varg);

	load_log_level();

	if ( level > liHigh )
	{
		level = liHigh;
	}

	if ( level < g_verbosity_level )
		return;

	check_for_tty();

	char timestr[64];
	time_t now = time(NULL);
	strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M", localtime(&now) );

	printf("%s %s (%s): %s\n", module, timestr, gLogLevelText[level], buffer);
	if ( msg )
	{
		printf("BMessage for last message:\n");
		msg->PrintToStream();
	}

	fflush(stdout);
}

void LOG(const char * module, log_importance level, const char *message, ...) {
	va_list varg;
	va_start(varg, message);
	char buffer[2048];
	vsprintf(buffer, message, varg);

	load_log_level();

	if ( level > liHigh )
	{
		level = liHigh;
	}

	if ( level < g_verbosity_level )
		return;

	check_for_tty();

	char timestr[64];
	time_t now = time(NULL);
	strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M", localtime(&now) );

	printf("%s %s (%s): %s\n", module, timestr, gLogLevelText[level], buffer);

	fflush(stdout);
}
