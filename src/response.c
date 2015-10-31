#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "response.h"
#include "screen.h"

#define IS_ENDLINE(p) (*p == '\r' || *p == '\n')

uint32_t g_status_code_map[1024] = {0};

static void get_header_value(const char *recv_buffer, const char *header, char *store) 
{
	char *p = NULL;
	char *start = NULL;
	char *end = NULL;
	char *line_start = strstr(recv_buffer, header);
	
	if (line_start != NULL) {
		p = line_start;
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

static uint32_t get_status_code(char *recv_buffer) 
{
	if (recv_buffer == NULL) {
		return 0;
	}
	if (strstr(recv_buffer, "HTTP/1.") == NULL) {
		return 0;
	}

	char *p = recv_buffer;
	while (!isspace(*p)) {
		p++;
	}

	return atoi(p);
}

int on_response(char *recv_buffer, int recv_len, http_response_t *response) 
{
	SCREEN(SCREEN_YELLOW, stdout, "New buffer");
	SCREEN(SCREEN_BLUE, stdout, "%s", recv_buffer);
	
	char content_length[128] = {0};

	response->status_code = get_status_code(recv_buffer);
	get_header_value(recv_buffer, "Server", response->server);
	get_header_value(recv_buffer, "Content-Length", content_length);
	response->content_length = atoi(content_length);

	if (response->status_code) {
		g_status_code_map[response->status_code]++;
	}

	return 0;
}

bool is_response_complete(conn_t *pconn, int recv_len) 
{
	char buffer[128] = {0};
	get_header_value(pconn->recv_buffer + pconn->offset, "Content-Length", buffer);
	int content_length = atoi(buffer);
	char *head_end = strstr(pconn->recv_buffer + pconn->offset, "\r\n\r\n");
	char *body_start = head_end + 4;

	if (content_length > 0) {
		if (body_start + content_length - pconn->recv_buffer <= recv_len) {
			int valid_length = body_start + content_length - pconn->recv_buffer;
			SCREEN(SCREEN_WHITE, stdout, "content length %d, recv_len %d, valid_len %d\n",  content_length, recv_len, valid_length);
			/* Process valid data */

			pconn->offset = recv_len - valid_length;
			memset(pconn->recv_buffer, 0, valid_length);	
			memcpy(pconn->recv_buffer, pconn->recv_buffer + valid_length, recv_len - valid_length);	

			return true;
		} else {
			return false;
		}
	/* Transfer-Encoding : trunked \r\n */
	} else {

		return false;
	}
}
