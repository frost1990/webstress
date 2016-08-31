#ifndef _REQUEST_
#define _REQUEST_

#include <stdint.h>
#include <stdbool.h>

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

typedef struct method_map {
	http_request_method_t method;
	const char *name;
} method_map;

typedef struct http_request {
	char scheme[16];
	char host[256];
	char path[256];
	char querystring[1024];
	char fragment[256];
	char userinfo[256];
	char content_type[256];
	char bodydata[1024];

	http_request_method_t method;
	int http_keep_alive;
	int connections;
	int content_length;
	int duration;
	/* Unit millisecond */
	int timeout;
	uint32_t ip;
	int port;

	bool pipelining;

	char *file_upload;
	char *additional_header;
	char *send_buffer;
} http_request;

void init_http_request(http_request *request);

void get_header_value(const char *header, const char *src, char *dst);

const char *get_method_name(http_request_method_t method); 

void parse_cli(int argc, char **argv, http_request *request);

int parse_opt(int argc, char **argv, http_request *request);

void compose_request_buffer(http_request* request);

void free_request_buffer(const http_request *request);

#endif
