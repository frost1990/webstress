#ifndef _VERSION_
#define _VERSION_

#include <stdio.h>
#include <stdlib.h>

char *version = "0.001";

void show_version() 
{
	printf("Version %s\n", version);
	exit(EXIT_SUCCESS);
}

#endif
