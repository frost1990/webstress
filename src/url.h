#ifndef _URL_
#define _URL_

#include "request.h"
#include "http_parser.h"

int parse_url(const char *url, struct http_parser_url *u, http_request *request);

#endif
