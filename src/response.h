#ifndef _RESPONSE_H_
#define _RESPONSE_H_

typedef struct http_request_t {
	char server[256];
	char content_type[256];
	char bodydata[1024];

	uint32_t status_code;
	uint32_t content_length;
} http_response_t;

int on_response(char *recv_buffer, int recv_len, http_response_t *response);

#endif
