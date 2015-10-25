#ifndef _CONFIG_
#define _CONFIG_

#define PORT_HTTP 	80
#define PORT_SSL_TLS 443	

typedef struct config {
	int connections;
	uint32_t ip;
	uint32_t port;
	char *additional_header;
	char scheme[16];
	char host[256];
	char path[256];
	char querystring[1024];
	char fragment[256];
} config;

int parse_opt(int argc, char **argv, config *request);

#endif
