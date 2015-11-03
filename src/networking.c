#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "screen.h"
#include "sknet.h"
#include "ev.h"
#include "networking.h"
#include "hash_conn.h"
#include "caculate.h"

/* Globals */
extern http_request myreq;
hash_conn_t ghash_conn;
stats_t net_record;

uint32_t g_status_code_map[1024] = {0};

static struct http_parser_settings parser_settings = {
	.on_message_complete = response_complete
};

int start_connection(int poller_fd, const http_request *request)
{
	hash_conn_init(&ghash_conn, request->connections);
	stats_init(&net_record);

	for (int i = 0; i < request->connections; i++) {
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0) {
			SCREEN(SCREEN_RED, stderr, "Cannot create socket: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sk_set_nonblock(fd);
		sk_tcp_no_delay(fd);
		sk_resue_addr(fd);
		/* Register this connection to the global hash table, allocate space for recieve message */
		hash_conn_add(&ghash_conn, fd);

		sk_async_ipv4_connect(poller_fd, fd, request->ip, request->port);
	}
	return 0;
}

int recieve_response(int poller_fd, int fd)
{
	int bytes = 0;
	conn_t *pconn = hash_conn_get(&ghash_conn, fd);
	if (pconn == NULL) {
		SCREEN(SCREEN_RED, stderr, "Fatal error, unable find socket %d's recieve buffer\n");
		exit(EXIT_FAILURE);
	}

	char *recv_buffer = pconn->recv_buffer + pconn->offset;
	while (true) {
		int ret = recv(fd, recv_buffer + bytes, RECV_BUFFER_SIZE * sizeof(char) - pconn->offset - bytes, 0);
		if (ret < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			 } else if (errno == EINTR) {	
				/* Interupted by a signal */
				continue;
			} else { 	
				SCREEN(SCREEN_RED, stderr, "recv(2): %s\n", strerror(errno));
				return RECV_ERROR;
			}
		} else if (ret == 0) {
			return 0;
		} else {
			bytes += ret;
		}
	}
	http_parser parser = pconn->parser;
	int total_bytes = bytes + pconn->offset;
	int nparsed = http_parser_execute(&parser, &parser_settings, pconn->recv_buffer, total_bytes);

	if (parser.upgrade) {
		return RECV_ERROR;
	} 

	if (nparsed > 0) {
		SCREEN(SCREEN_BLUE, stdout, "recv bytes %d, offset %d, total_bytes %d, nparsed %d, httperrno %s, buffer %p\n", bytes, pconn->offset, 
				total_bytes, nparsed, http_errno_name(parser.http_errno), pconn->recv_buffer);
		if (parser.http_errno == HPE_CB_message_complete) {
			memset(pconn->recv_buffer, 0, total_bytes);
			pconn->offset = 0;
			/* Tell the ev_loop_poller to notify writable events and send next request */
			return RECV_NEXT;
		}

		pconn->offset += bytes;
		return bytes;
	} else {	
		SCREEN(SCREEN_YELLOW, stdout, "recv bytes %d, offset %d, total_bytes %d, nparsed %d, httperrno %s, buffer %p\n", bytes, 
			pconn->offset, total_bytes, nparsed, http_errno_name(parser.http_errno), pconn->recv_buffer);
		return RECV_ERROR;
	}
}

int send_request(int poller_fd, int fd) 
{	
	conn_t *pconn = hash_conn_get(&ghash_conn, fd);
	if (pconn == NULL) {
		SCREEN(SCREEN_RED, stderr, "Fatal error, unable to find socket %d's recieve buffer\n", fd);
		exit(EXIT_FAILURE);
	}

	int offset = 0;
	char *send_buffer = myreq.send_buffer;
	int len = strlen(send_buffer);
	while (true) {   
		int ret = send(fd, send_buffer + offset, strlen(send_buffer) - offset, 0);
		if (ret < 0) {   
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;	
			} else if (errno == EINTR) {
				/* Interupted by a signal */
				continue;
			} else {
				SCREEN(SCREEN_RED, stderr, "send(2): %s\n", strerror(errno));
				return SEND_ERROR;
			}
		}  
		len -= ret;
		offset += ret;
		/* All data sent to socket's sending buffer */
		if (len <= 0) {   
			break;
		}	 
	}

	net_record.total_requests++;
	/* Record send time */
	gettimeofday(&(pconn->latest_snd_time), NULL);
	return offset;
}

int close_connection(int poller_fd, int fd)
{
	/* Free recv_buffer, free conn_t node  */
	hash_conn_delete(&ghash_conn, fd);
	ev_del_event(poller_fd, fd);
	sk_close(fd);
	return 0;
}

int reconnect(int poller_fd, uint32_t ip, int port) 
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		SCREEN(SCREEN_RED, stderr, "Cannot create socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sk_set_nonblock(fd);
	sk_tcp_no_delay(fd);
	sk_resue_addr(fd);
	/* Register this connection to the global hash table, allocate space for recieving message */
	hash_conn_add(&ghash_conn, fd);

	return sk_async_ipv4_connect(poller_fd, fd, ip, port);
}

void free_conn_rcv_buffer(conn_t *pconn)
{
	free(pconn->recv_buffer);
}

int response_complete(http_parser *parser) 
{
	SCREEN(SCREEN_GREEN, stdout, "on_message_complete, nread=%d\n", parser->nread);
	conn_t *pconn = parser->data;

	uint32_t status_code = parser->status_code;
	g_status_code_map[status_code]++;
	net_record.total_responses++;
	struct timeval now;
	gettimeofday(&now, NULL);
	uint32_t cost = stats_get_interval(&(pconn->latest_snd_time), &now);
	stats_add(&net_record, cost);

	http_parser_init(parser, HTTP_RESPONSE);
	/* Tell parser to stop parsing and reset */
	return RECV_NEXT;
}

