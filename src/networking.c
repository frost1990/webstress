#include <stdlib.h>
#include <string.h>

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
