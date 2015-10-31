#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "response.h"
#include "http_parser.h"

#define IS_ENDLINE(p) (*p == '\r' || *p == '\n')

static void get_header_value(const char *recv_buffer, const char *header, char *store) 
{
	char *p = NULL;
	char *start = NULL;
	char *end = NULL;
	char *line_start = strstr(recv_buffer, header);
	
	if (line_start != NULL) {
		while (!IS_ENDLINE(p) && *p != ':') {
			p++;
		}
		
		/* Illegal header format */
		if (IS_ENDLINE(p)) {
			return;
		}
		p++;
		while (isblank(*p)) {
			p++;
		}

		/* Illegal header format */
		if (IS_ENDLINE(p)) {
			return;
		}
		start = p;

		while (!isspace(*p)) {
			p++;
		}
		end = p - 1;
		strncpy(store, start, end - start + 1);
	}

	return;
}

int on_response(char *recv_buffer, int recv_len, http_response_t *response) 
{
	http_parser_settings settings;
	size_t nparsed = 0;

	http_parser *parser = (http_parser *) malloc(sizeof(http_parser));
	http_parser_init(parser, HTTP_RESPONSE);

	nparsed = http_parser_execute(parser, &settings, recv_buffer, recv_len);

	if (parser->upgrade) {
	/* Handle new protocol */
	} else if (nparsed != recv_len) {
	/* Handle error. Usually just close the connection. */
		free(parser);
		return -1;
	}

	response->status_code = parser->status_code;
	printf("Status_code: %u\n", response->status_code);
	response->content_length = parser->content_length;
	get_header_value(recv_buffer, "Server", response->server);

	free(parser);
	return 0;
}
