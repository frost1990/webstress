#ifndef _URL_
#define _URL_

#include "request.h"
#include "http_parser.h"

void urlencode(unsigned char *src, int  src_len, unsigned char *dest, int  dest_len);

unsigned char* urldecode(unsigned char* encode, unsigned char* decode);

int parse_url(const char *url, struct http_parser_url *u, http_request *request);

#endif
