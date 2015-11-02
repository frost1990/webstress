#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include <time.h>
#include <sys/time.h>

#include "request.h"
#include "http_parser.h"

#define RECV_BUFFER_SIZE (150 * 1024)

typedef struct conn_t {
	struct timeval latest_snd_time; 
	int fd;
	char *recv_buffer;
	int offset;
	int parsed_bytes;
} conn_t;

int start_connection(int poller_fd, const http_request *request);

int recieve_response(int poller_fd, int fd);

int send_request(int poller_fd, int fd);

int close_connection(int poller_fd, int fd);

int reconnect(int poller_fd, uint32_t ip, int port);

void free_conn_rcv_buffer(conn_t *pconn);

int parse_response(conn_t *pconn, http_parser *parser);

int response_complete(http_parser *parser); 


#endif
