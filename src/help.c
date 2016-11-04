#include <stdio.h>

#include "help.h"
#include "version.h"
#include "screen.h"

const char *help_message = 
"\nUSAGE: webstress [options] <url>\n"
"\nOPTIONS:\n"
"	-b			Specify the file you want to update, Content-Type will be application/octet-stream\n"
"	-c			Concurrent tcp connections\n"
"	-d			Add post application/x-www-form-urlencoded data\n"
"	-f			Run in pipelining mode, will send as many requests as possible,	some summary information is not available\n"
"	-H			Add additional http request header\n"
"	-h			Print help message\n"
"	-t			Benchmark duration time in seconds\n"
"	-p			Specify a port\n"
"	-v			Show version information\n";	 

void help(void) 
{
	SCREEN(SCREEN_WHITE, stdout, "%s\n", help_message);
}
