#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "request.h"
#include "http_parser.h"
#include "sknet.h"
#include "help.h"
#include "version.h"
#include "regex_util.h"
#include "url.h"

/* The array must be correctly ordered */
static struct method_map method_name_map [] = {
	{GET, "GET"},
	{HEAD, "HEAD"},
	{PUT, "PUT"},
	{POST, "POST"},
	{TRACE, "TRACE"},
	{OPTIONS, "OPTIONS"},
	{DELETE, "DELETE"},
};

const char *get_method_name(http_request_method_t method) 
{
	for (int i = 0; i < DELETE - GET + 1; i++) {
		if (method == method_name_map[i].method) {
			return method_name_map[i].name;
		}
	}
	return NULL;
}

void init_http_request(http_request *request)
{
	request->method = GET;
	request->connections = 1;
	request->duration = 0;
	request->timeout = 0;
	request->http_keep_alive = HTTP_KEEP_ALIVE;
	request->ip = 0;
	request->port = PORT_HTTP;
	request->additional_header = NULL;
	memset(request->scheme, 0, 16);
	memset(request->host, 0, 256);
	memset(request->path, 0, 256);
	memset(request->querystring, 0, 1024);
	memset(request->fragment, 0, 256);
	memset(request->bodydata, 0, 1024);
}

int parse_opt(int argc, char **argv, http_request *request) 
{
	if (argc == 1) {  
		help();
		return 0;
	}

  	struct http_parser_url us;
	char *src_url = argv[argc - 1];
  	
	if (parse_url(src_url, &us, request) != 0) {
		exit(EXIT_FAILURE);
	}

	/* Check whether the host field is a direct ipv4 address */
	request->ip = sk_get_host_ipv4(request->host); 
	char ipstr[64] = {0};
	sk_ipv4_tostr(request->ip, ipstr, strlen(ipstr));
	if (request->ip == 0) {
		printf("Could not resolve host: %s\n", request->host);
		exit(0);
	}
	printf("Host: %s, ip: %s\n", request->host, ipstr);
	request->port = PORT_HTTP; 
	request->method = GET; 

	int ch;                     
	while ((ch = getopt(argc, argv, "c:d:H:hm:t:v")) != -1) {
		switch(ch) {
			case 'c':
				request->connections = atoi(optarg);
				break;
			case 'd':
				request->method= POST; 
				strncpy(request->bodydata, optarg, 1024);
				break;
			case 'H':
				request->additional_header = optarg;
				break;
			case 'h':
				help();
				break;
			case 'm':
				request->duration = 60 * atoi(optarg);
				break;
			case 'v':
				show_version();
				break;
			case '?':
				break;
			case ':':
				break;
			default:	
				break;
		}
	}

	return 0;
}

char *compose_request_buffer(const http_request* request)
{
	char *buffer = malloc(REQUEST_BUFFER_SIZE * sizeof(char));
	if (buffer == NULL) {
		return NULL;
	}

	memset(buffer, 0, REQUEST_BUFFER_SIZE);
	size_t offset = 0;
	
	/* Request line */
	int bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s", get_method_name(request->method));
	offset += bytes;

	/* Path, querystring, fragment */
	if (strlen(request->path) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s", request->path);
		offset += bytes;
	}

	if (strlen(request->querystring) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "?%s", request->querystring);
		offset += bytes;
	}

	if (strlen(request->fragment) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "#%s ", request->fragment);
		offset += bytes;
	}

	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s\r\n", HTTP_1_1);
	offset += bytes;
	/* Request line ends here */

	if (request->http_keep_alive == HTTP_KEEP_ALIVE) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Connection: keep-alive\r\n");
		offset += bytes;
	}

	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s\r\n", request->additional_header);
	offset += bytes;

	/* Header ends here */
	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "\r\n");
	offset += bytes;

	/* Body starts */
	if (strlen(request->bodydata) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s", request->bodydata);
		offset += bytes;
	}

	return buffer;
}

void free_request_buffer(char *buffer) 
{
	free(buffer);
	return;
}
