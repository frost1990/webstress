#ifndef _CONFIG_
#define _CONFIG_

#define PORT_HTTP 	80
#define PORT_SSL_TLS 443	

typedef struct config {
	int connections;
	uint32_t ip;
	uint32_t port;
	char *basic_url;
	char *additional_header;
} config;

int parse_opt(int argc, char **argv, config *request);

#endif
