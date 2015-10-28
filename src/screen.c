#include <stdarg.h>

#include "screen.h"

int screen_print(const char *color, FILE *fp, const char *format, ...)  
{
	va_list args;
	va_start(args, format);
	int ret = vfprintf(fp, format, args);
	va_end(args);
	return ret;
}
