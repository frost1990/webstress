#include <stdio.h>
#include <stdlib.h>

#include "version.h"

static char *version = "0.001";

void show_version() 
{
	printf("Version %s\n", version);
	exit(EXIT_SUCCESS);
}
