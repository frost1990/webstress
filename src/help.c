#include <stdio.h>
#include "help.h"
#include "version.h"

const char *help_message = 
"\nUSAGE: webstress [options] <url>\n"
"\nOPTIONS:\n"
"	-c			Concurrent tcp connections\n"
"	-d			Add post form data\n"
"	-f			Run in pipelining mode, will send as many requests as possible,	some summary information are not available\n"
"	-H			Add additional http request header\n"
"	-h			Print help message\n"
"	-m			Benchmark duration time in minutes\n"
"	-p			Specify a port\n"
"	-t			Set request process timeout in milliseconds and calculate time out rate\n" 
"	-v			Show version information\n";	 

void help(void) 
{
	printf("%s\n", help_message);
}
