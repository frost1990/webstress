#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "request.h"
#include "http_parser.h"

#define RECV_BUFFER_SIZE (64 * 1024)

#define RECV_ERROR -1
/* When RECV_NEXT is returned, flush the buffer and notify the writable event to send next http request */
#define RECV_NEXT -2
#define SEND_ERROR -1

typedef struct conn_t {
	struct timeval latest_snd_time; 
	int fd;
	char *recv_buffer;
	int offset;
	int capacity;
	bool connected;
	http_parser parser;
} conn_t;

/* Record all the abnormal events, such as recieved an RST segment from peer */
typedef struct net_event_record_t {
	uint32_t fin_times;
	uint32_t rst_times;
	uint32_t timeout_times;
	uint32_t snd_bytes;
	uint32_t rcv_bytes;
} net_event_record_t;

int start_connection(int poller_fd, const http_request *request);

int recieve_response(int poller_fd, int fd);

int send_request(int poller_fd, int fd);

int close_connection(int poller_fd, int fd);

int reconnect(int poller_fd, uint32_t ip, int port);

void free_conn_rcv_buffer(conn_t *pconn);

int parse_response(conn_t *pconn, http_parser *parser);

int response_complete(http_parser *parser); 

#endif
