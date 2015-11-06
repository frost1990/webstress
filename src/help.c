#include <stdio.h>

#include "help.h"
#include "version.h"
#include "screen.h"

const char *help_message = 
"\nUSAGE: webstress [options] <url>\n"
"\nOPTIONS:\n"
"	-c			Concurrent tcp connections\n"
"	-d			Add post application/x-www-form-urlencoded data\n"
"	-f			Run in pipelining mode, will send as many requests as possible,	some summary information are not available\n"
"	-H			Add additional http request header\n"
"	-h			Print help message\n"
"	-m			Benchmark duration time in minutes\n"
"	-p			Specify a port\n"
"	-v			Show version information\n";	 

void help(void) 
{
	SCREEN(SCREEN_WHITE, stdout, "%s\n", help_message);
}
