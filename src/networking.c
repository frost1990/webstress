#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "screen.h"
#include "sknet.h"
#include "ev.h"
#include "networking.h"

int start_connection(int poller_fd, const http_request *request)
{
	for (int i = 0; i < request->connections; i++) {
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0) {
			SCREEN(SCREEN_RED, stderr, "socket %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sk_set_nonblock(fd);
		sk_tcp_no_delay(fd);
		sk_resue_addr(fd);
		sk_async_ipv4_connect(poller_fd, fd, request->ip, request->port);
	}
	return 0;
}

int recieve_response(int poller_fd, int fd)
{
	int bytes = 0;
	char *recv_buffer = NULL;
	while (true) {
		int ret = recv(fd, recv_buffer + bytes, 1024, 0);
		if (ret < 0)  {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			 } else if (errno == EINTR) {	/* Interupted by a signal */
				continue;
			} 	
			return -1;
		} else if (ret == 0) {
			return -1;
		} else {
			bytes += ret;
		}
	}

	return 0;
}

int send_request(int poller_fd, int fd) 
{
	size_t offset = 0;
	char *send_buffer = NULL;
	int len = strlen(send_buffer);
	while (true) {   
		int ret = send(fd, send_buffer + offset, strlen(send_buffer) - offset, 0);
		if (ret < 0) {   
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;	
			} else if (errno == EINTR) {
				continue;
			}
		}  
		len -= ret;
		offset += ret;
		/* All data sent to socket's sending buffer */
		if (len <= 0) {   
			break;
		}	 
	}
	return 0;
}

int close_connection(int poller_fd, int fd)
{

	sk_close(fd);
	return 0;
}
