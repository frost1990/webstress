#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdio.h>
#include <string.h>

/* Font color */
#define SCREEN_BLACK 			"30m"
#define SCREEN_RED				"31m"
#define SCREEN_GREEN 			"32m"
#define SCREEN_YELLOW 			"33m"
#define SCREEN_BLUE 			"34m"
#define SCREEN_PURPLE 			"35m"
#define SCREEN_DARK_GREEN 		"36m"
#define SCREEN_WHITE 			"37m"

/* Background color */
#define SCREEN_BG_BLACK 		"40"
#define SCREEN_BG_RED 			"41"
#define SCREEN_BG_GREEN 		"42"
#define SCREEN_BG_BLUE 			"43"
#define SCREEN_BG_YELLOW 		"44"
#define SCREEN_BG_PURPLE 		"45"
#define SCREEN_BG_DARK_GREEN 	"46"
#define SCREEN_BG_WHITE 		"47"

#define CONCAT(a, b)  a b 

#define COLOR_SET(color) CONCAT("\033[", color)
#define COLOR_DISABLE "\033[0m"

int screen_print(const char *color, FILE *fp, const char *format, ...); 

#define SCREEN(color, fp, format, vargs...) do { \
	char fmt[2048] = {0}; \
	strncat(fmt, COLOR_SET(color), 2048 - strlen(fmt)); \
	strncat(fmt, format, 2048 - strlen(fmt)); \
	strncat(fmt, COLOR_DISABLE, 2048 - strlen(fmt)); \
	screen_print(color, fp, fmt, ##vargs); \
} while (0);

#endif
