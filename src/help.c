#include <stdio.h>
#include "help.h"
#include "version.h"

const char *help_message = 
"\nUSAGE: webpress [options] <url>\n"
"\nOPTIONS:\n"
"	-c						Concurrent tcp connections\n"
"	-d						Add post form data\n"
"	-H						Add additional http request header\n"
"	-h						Print help message\n"
"	-m						Benchmark duration time in minutes\n"
"	-t						Set request process timeout in milliseconds and calculate time out rate\n" 
"	-v						Show version information\n";	 

void help(void) 
{
	printf("%s\n", help_message);
}
