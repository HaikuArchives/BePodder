#include <ctype.h>
#include <stdlib.h>
#include <UTF8.h>

#include <Debug.h>
#include "htmlparse.h"

void encode_html( BString &msg ) {
	// BStrings are slow and sucky, but this is real easy
	msg.ReplaceAll("&","&amp;");
	msg.ReplaceAll("\"","&quot;");
	msg.ReplaceAll("<","&lt;");
	msg.ReplaceAll(">","&gt;");
	
	msg.ReplaceAll("[b]","<b>");
	msg.ReplaceAll("[/b]","</b>");
	msg.ReplaceAll("[i]","<i>");
	msg.ReplaceAll("[/i]","</i>");
	
	msg.Prepend("<html>");
	msg.Append("</html>");
}

int 
parse_html_2( char * msg, int size, 	char** to, int* tosize ){

	return 0;
}


int parse_html( char * msg , int size )
{
	
	//-- pre parser --//
	BString preparser(msg);
//  preparser.ReplaceAll("\n"," ");
	preparser.ReplaceAll("\r"," ");
	preparser.ReplaceAll("\t"," ");
	preparser.ReplaceAll("&agrave;","à"); //should be illegal!
	preparser.ReplaceAll("&egrave;","è"); //should be illegal!
	preparser.ReplaceAll("&nbsp;"," "); //should be illegal!
	preparser.ReplaceAll("&amp;","&");
	preparser.ReplaceAll("&quot;","\"");
	preparser.ReplaceAll("&gt;",">");
	preparser.ReplaceAll("&lt;","<");
	preparser.ReplaceAll("&apos;","'");	
	preparser.CopyInto(msg,0,preparser.Length());
	size = preparser.Length();
	//------------------//
	
	if(msg[size]!='\0') msg[size]='\0';
	
	bool is_in_tag = false;
	bool is_newline = true;
	bool is_last_space = false;
	int copy_pos = 0;
	
	char * copy = new char[size + 1];
	
	memset(copy,'\0',size + 1);
	
	
	for ( int i=0; msg[i]; i++ )
	{
		switch ( msg[i] )
		{
			case ' ':
				if(!is_newline && !is_last_space) {
					copy[copy_pos++] = msg[i];
					is_last_space=true;
				}
				
			break;
			
			case '\n':
				is_newline = true;
				is_last_space=false;
				copy[copy_pos++] = ' ';
			break;
			case '<':
				is_last_space=false;
				is_in_tag = true;
				/*for (int j = i+1; msg[j]; j++) {
					if (isspace(msg[j])) continue;
					else if (tolower(msg[j]) == 'a') {
						copy[copy_pos++] = '[';
						copy[copy_pos++] = ' ';
						for (; msg[j] && msg[j] != '='; j++); j++;
						for (; msg[j] && isspace(msg[j]); j++);
						if (msg[j] == '\"') j++;
						for (; msg[j] && !isspace(msg[j]) && msg[j] != '\"'; j++)
								copy[copy_pos++] = msg[j];
						copy[copy_pos++] = ' ';
						copy[copy_pos++] = ']';
						copy[copy_pos++] = ' ';
					} else break;
				}
				*/
				//while(		
				is_newline = false;	
				break;
			case '>':
				is_in_tag = false;
				is_newline = false;	
				is_last_space=false;
				break;
			case '&':
				is_newline = false;	
				is_last_space=false;
				if (strncmp("&quot;",&msg[i],6) == 0) {
					copy[copy_pos++] = '\"';
					i += 5;
					break;
				}
				if (strncmp("&lt;",&msg[i],4) == 0) {
					copy[copy_pos++] = '<';
					i += 3;
					break;
				}
				if (strncmp("&gt;",&msg[i],4) == 0) {
					copy[copy_pos++] = '>';
					i += 3;
					break;
				}
				if (strncmp("&amp;",&msg[i],5) == 0) {
					copy[copy_pos++] = '&';
					i += 4;
					break;
				}
				if (strncmp("&#",&msg[i],2) == 0) {
					
											
		
				//test code:
					
								unsigned n = 0, nb, j;
								char bytes[6], b0;
					
								/* properties of UTF-8 chars >= a given value */
								static struct { unsigned minval; int len; char byte0; }
								utf8prop[] = {
									{ 1<<26, 6, 0xfc },
									{ 1<<21, 5, 0xf8 },
									{ 1<<16, 4, 0xf0 },
									{ 1<<11, 3, 0xe0 },
									{ 1<<7,  2, 0xc0 },
									{ 0,     1, }
								};
					
								char* buf=msg;
							
							i += 2;
							n = 0;
							if (buf[i] == 'x' || buf[i] == 'X') {
								for (i++; buf[i] != ';'; i++) {
									const char *hex = "0123456789abcdef",
										  			*hex2 = "0123456789ABCDEF";
									char *s;
									int c;
									if ((s = strchr(hex, buf[i])))
										c = s - hex;
									else if ((s = strchr(hex2, buf[i])))
										c = s - hex2;
									else
										goto invalid;
				
									n = (n << 4) | c;
								}
							}
							else while (isdigit(buf[i]))
								n = n*10+(buf[i++] - '0');
							if (!n || n > 1<<31 || buf[i] != ';') {
							    invalid:
								//fprintf(stderr, "bad input sequence\n");
								break;
							}
							/* utf-8 encoding starts here */
							if (n < 0x80) {
								copy[copy_pos++] = n;
								break;
							}
							for (j = 0; n < utf8prop[j].minval; j++)
								;
							nb = utf8prop[j].len;
							b0 = utf8prop[j].byte0;
				
							for (j = nb; j; j--) {
								bytes[j-1] = (n & 0x3f) | 0x80;
								n >>= 6;
							}
							bytes[0] |= b0;
				
							for (j = 0; j < nb; j++)
								copy[copy_pos++] = bytes[j];
					
					break;
				}
			default:
				is_newline = false;
				is_last_space=false;	
				if ( !is_in_tag )
				{
					copy[copy_pos++] = msg[i];
				}
		}
	}
	
	copy[copy_pos] = '\0';
	
	
	strncpy(msg, copy,copy_pos);
	

	delete copy;
	return copy_pos;
}
