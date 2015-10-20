#ifndef _HELP_
#define _HELP_

#include <stdio.h>
#include "version.h"

const char *help_message = 
"\nUSAGE: webpress [options] <url>\n"
"\nOPTIONS:\n"
	"-h						Print help message\n"
	"-v						Show version infomation\n"	 
	"-H						Add additional http request header\n"
	"-c						Concurrent tcp connections\n";

void help(void) 
{
	printf("%s\n", help_message);
}

#endif
