#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "request.h"
#include "http_parser.h"
#include "sknet.h"
#include "help.h"
#include "version.h"
#include "regex_util.h"
#include "url.h"
#include "screen.h"
#include "exception.h"

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

void get_header_value(const char *header, const char *src, char *dst)
{
	if (src == NULL) {
		return;
	}

	char *p = NULL;
	if ((p = strstr(src, header)) == NULL) {
		return;
	}

	char *val = strstr(p + strlen(header), ":") + strlen(":"); 
	while (isspace(*val)) {
		val++;
	}
	char *start = val;
	while (!isspace(*val)) {
		val++;
	}

	/*Host: 12345\r\n */
	snprintf(dst, val - start, "%s", start);
	return;
}	

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
	request->pipelining = false;
	request->additional_header = NULL;
	request->send_buffer = NULL;
	memset(request->scheme, 0, 16);
	memset(request->host, 0, 256);
	memset(request->path, 0, 256);
	memset(request->querystring, 0, 1024);
	memset(request->fragment, 0, 256);
	memset(request->content_type, 0, 256);
	memset(request->bodydata, 0, 1024);
}

void parse_cli(int argc, char **argv, http_request *request) {
	int ch;                     
	while ((ch = getopt(argc, argv, "c:d:fH:hm:p:t:v")) != -1) {
		switch(ch) {
			case 'c':
				if (atoi(optarg) < 1) {
					SCREEN(SCREEN_RED, stderr, "Connection number must be equal to or greater than one.\n");
					exit(EXIT_FAILURE);	
				} 

				if (atoi(optarg) > 65535) {
					SCREEN(SCREEN_RED, stderr, "Connection number cannot exceed ephemeral port range.\n");
					exit(EXIT_FAILURE);	
				}
				request->connections = atoi(optarg);

				SCREEN(SCREEN_YELLOW, stdout, "TCP connections:\t");
				SCREEN(SCREEN_DARK_GREEN, stdout, "%d\n", request->connections);
				break;
			case 'd':
				request->method= POST; 
				strncpy(request->bodydata, optarg, 1024);
				strncpy(request->content_type, "application/x-www-form-urlencoded", 1024);
				break;
			case 'f':
				request->pipelining = true;
				break;
			case 'H':
				request->additional_header = optarg;
				break;
			case 'h':
				help();
				break;
			case 'm':
				if (atoi(optarg) < 1) {
					printf("Duration time must be equal to or greater than one minute.\n");
					exit(EXIT_FAILURE);	
				}
				request->duration = 60 * atoi(optarg);
				break;
			case 'p':
				if (atoi(optarg) < 1 || atoi(optarg) > 65535) {
					SCREEN(SCREEN_RED, stderr, "Invalid port number.\n");
					exit(EXIT_FAILURE);	
				} 

				request->port = atoi(optarg);
				break;
			case 't':
				if (atoi(optarg) < 1) {
					printf("Request-response time out must be equal to or greater than one millisecond.\n");
					exit(EXIT_FAILURE);	
				}
				request->timeout = atoi(optarg);
				break;
			case 'v':
				show_version();
				break;
			case '?':
				exit(EXIT_FAILURE);
				break;
			case ':':
				break;
			default:	
				break;
		}
	}
}

int parse_opt(int argc, char **argv, http_request *request) 
{
	if (argc == 1) {  
		help();
		exit(EXIT_SUCCESS);
	}

	parse_cli(argc, argv, request);

	char *src_url = argv[optind];
	/* In this case, the first argument is the request url, just like: ./webpress google.com -c 100 */
	if (optind == 1) {
		parse_cli(argc - 1, argv + 1, request);
	}

	if (optind == argc) {
		SCREEN(SCREEN_RED, stderr, "Please input your request url.\n");
		exit(EXIT_FAILURE);
	}

	struct http_parser_url us;
  	
	if (parse_url(src_url, &us, request) != 0) {
		exit(EXIT_FAILURE);
	}

	request->ip = sk_get_host_ipv4(request->host); 
	char ipstr[64] = {0};
	sk_ipv4_tostr(request->ip, ipstr, strlen(ipstr));
	if (request->ip == 0) {
		SCREEN(SCREEN_RED, stderr, "Could not resolve host: %s\n", request->host);
		exit(EXIT_FAILURE);
	}
	SCREEN(SCREEN_YELLOW, stdout, "Host:\t\t\t");
	SCREEN(SCREEN_DARK_GREEN, stdout, "%s\n", request->host);
	SCREEN(SCREEN_YELLOW, stdout, "Address:\t\t");
	SCREEN(SCREEN_DARK_GREEN, stdout, "%s:", ipstr);
	SCREEN(SCREEN_DARK_GREEN, stdout, "%d\n", request->port);

	return 0;
}

void compose_request_buffer(http_request* request)
{
	char *buffer = malloc(REQUEST_BUFFER_SIZE * sizeof(char));
	ASSERT_ALLOCATE(buffer);

	memset(buffer, 0, REQUEST_BUFFER_SIZE);
	size_t offset = 0;
	
	/* Start line */
	int bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s ", get_method_name(request->method));
	offset += bytes;

	/* Path, querystring, fragment */
	if (strlen(request->path) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "/%s", request->path);
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

	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, " %s\r\n", HTTP_1_1);
	offset += bytes;
	/* Start line ends here */
	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Accept: *\r\n");
	offset += bytes;

	/* Host */
	if (is_match_pattern(request->host, REGEX_IPV4) != 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Host: %s:\r\n", request->host);
		offset += bytes;
	} else {
		char host[256] = {'\0'};
		get_header_value("Host", request->additional_header, host);
		if (strlen(host) == 0) {
			if (request->port == PORT_HTTP) {
				bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Host: %s:\r\n", request->host);
				offset += bytes;
			} else { 
				bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Host: %s:%d\r\n", request->host, request->port);
				offset += bytes;
			}
		}
	}

	if (strlen(request->content_type) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Content-Type: %s\r\n", request->content_type);
		offset += bytes;
	}

	if (request->http_keep_alive == HTTP_KEEP_ALIVE) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "Connection: keep-alive\r\n");
		offset += bytes;
	}

	if (request->additional_header != NULL) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s\r\n", request->additional_header);
		offset += bytes;
	}

	/* Header ends here */
	bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "\r\n");
	offset += bytes;

	SCREEN(SCREEN_BLUE, stdout, "Http request header:\n");
	SCREEN(SCREEN_GREEN, stdout, "%s", buffer);

	/* Body starts */
	if (strlen(request->bodydata) > 0) {
		bytes = snprintf(buffer + offset, REQUEST_BUFFER_SIZE - offset, "%s", request->bodydata);
		offset += bytes;
	}

	request->send_buffer = buffer;
}

void free_request_buffer(const http_request *request) 
{
	free(request->send_buffer);
	return;
}
