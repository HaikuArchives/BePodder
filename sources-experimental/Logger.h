#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <String.h>
#include <Message.h>
#include <string>

#define TRACE_CLASS		Tracer tr(__PRETTY_FUNCTION__)
class Tracer
{
	private:
		BString str;
		
	public:
		Tracer( const char * _str ) 
		: str(_str) 
		{ 
			printf("%s\n",str.String()); 
		};
		~Tracer() 
		{ 
			printf("-%s\n",str.String() );
		};
};

enum log_importance {
	liDebug = 0,	// debug
	liLow,			// lots and lots of messages
	liMedium,		// enough to follow what's happening
	liHigh,			// general idea
	liQuiet = 100	// Don't say a word. Don't use this when calling LOG(), it's for reducing output.
};

extern void LOG( const char * module, log_importance level, const char * msg, const BMessage *, ...);
extern void LOG( const char * module, log_importance level, const char * msg, ...);

// if level < g_verbosity_level then LOG() doesn't print the msg
extern log_importance g_verbosity_level;

#endif
