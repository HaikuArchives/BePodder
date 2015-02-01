#ifndef IMKIT_HTML_PARSE_H
#define IMKIT_HTML_PARSE_H

#include <String.h>

int parse_html( char * msg, int size );

int parse_html_2( char * msg, int size, 	char** to, int* tosize );

void encode_html( BString &msg );

#endif // IMKIT_HTML_PARSE_H
