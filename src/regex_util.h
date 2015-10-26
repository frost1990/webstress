#ifndef _REGEX_UTIL_H_
#define _REGEX_UTIL_H_

#define REGEX_IPV4 "((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))"
#define REGEX_URL_SCHEME "^[a-zA-Z][0-9a-zA-Z\\.-]*:\\/\\/.*"

int is_match_pattern(const char *src, const char *pattern); 

#endif 
