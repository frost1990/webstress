#ifndef _REQUEST_
#define _REQUEST_

#include <stdint.h>

#define HTTP_1_1 "HTTP/1.1"

#define PORT_HTTP 	80
#define PORT_SSL_TLS 443	

#define HTTP_KEEP_ALIVE 1
#define HTTP_NO_KEEP_ALIVE 2

#define REQUEST_BUFFER_SIZE 4096

typedef enum {
	GET = 100,
	HEAD,
	PUT,
	POST,
	TRACE,
	OPTIONS,
	DELETE,
} http_request_method_t;

typedef struct method_map{
	http_request_method_t method;
	const char *name;
} method_map;

typedef struct http_request{
	http_request_method_t method;
	int http_keep_alive;
	int connections;
	uint32_t ip;
	uint32_t port;
	char *additional_header;
	char scheme[16];
	char host[256];
	char path[256];
	char querystring[1024];
	char fragment[256];
} http_request;

void init_http_request(http_request *request);

const char *get_method_name(http_request_method_t method); 

int parse_opt(int argc, char **argv, http_request *request);

char *compose_request_buffer(const http_request* request);

void free_request_buffer(char *buffer);

#endif
