#ifndef _CONFIG_
#define _CONFIG_

typedef struct config {
	int connections;
	char *basic_url;
	char *additional_header;
} config;

int parse_opt(int argc, char **argv);

#endif
