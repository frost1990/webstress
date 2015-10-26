#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "regex_util.h"

int is_match_pattern(const char *src, const char *pattern) 
{
	int ret = -1;
	regex_t reg;

	ret = regcomp(&reg, pattern, REG_EXTENDED);
	if (ret != 0) {
		char error[1024] = {0};
		regerror(ret, &reg, error, strlen(error));
		regfree(&reg);
		return -1;
	}
	
	ret = regexec(&reg, src, 0, NULL, 0);
	regfree(&reg);
	return ret;
}
