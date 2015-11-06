#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <stdlib.h>
#include <unistd.h>

#include "screen.h"

#define ASSERT_ALLOCATE(p) do { \
	if (p == NULL) { \
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) or calloc(3) failed.\n"); \
		exit(EXIT_FAILURE); \
	} \
} while (0); 

#endif
