#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdbool.h>

#include "networking.h"

typedef struct http_response_t {
	char server[256];
	char content_type[256];
	char bodydata[1024];

	uint32_t status_code;
	uint32_t content_length;
} http_response_t;

int on_response(char *recv_buffer, int recv_len, http_response_t *response);

int is_response_complete(conn_t *pconn, int total_len);

#endif
