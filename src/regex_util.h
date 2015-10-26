#ifndef _REGEX_UTIL_H_
#define _REGEX_UTIL_H_

#define REGEX_IPV4 "((([0-9]{1,2})|(1[0-9]{2})|(2[0-4][0-9])|(25[0-5]))\\.){3}(([0-9]{1,2})|([0-9]{2})|(2[0-4][0-9])|(25[0-5]))"
#define REGEX_URL_SCHEME "^[a-zA-Z][0-9a-zA-Z\\.-]*:\\/\\/.*"

int is_match_pattern(const char *src, const char *pattern); 

#endif 
