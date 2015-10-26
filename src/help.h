#ifndef _HELP_
#define _HELP_

#include <stdio.h>
#include "version.h"

const char *help_message = 
"\nUSAGE: webpress [options] <url>\n"
"\nOPTIONS:\n"
	"-c						Concurrent tcp connections\n"
	"-h						Print help message\n"
	"-H						Add additional http request header\n"
	"-m						Benchmark duration time in minutes\n"
	"-v						Show version information\n";	 

void help(void) 
{
	printf("%s\n", help_message);
}

#endif
