#include <stdlib.h>
#include <stdint.h>

#include "response.h"
#include "http_parser.h"

int on_response(char *recv_buffer, int recv_len, http_response_t *response) {
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
	response->content_length = parser->content_length;

	free(parser);
	return 0;
}
