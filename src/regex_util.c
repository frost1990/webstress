#include <regex.h>
#include <stdio.h>

#include "regex_util.h"

int is_match_pattern(const char *src, const char *pattern) 
{
	int ret = -1;
	regex_t reg;

	if (regcomp(&reg, pattern, REG_EXTENDED) != 0) {
		printf("Failed to compile regular expression %s\n", pattern);
		regfree(&reg);
		return -1;
	}
	
	ret = regexec(&reg, src, 0, NULL, 0);

	regfree(&reg);
	return ret;
}
