#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "request.h"
#include "screen.h"
#include "networking.h"
#include "sknet.h"
#include "ev.h"

long long request_seq = 0;
long long response_seq = 0;
struct timeval start;
struct timeval end;

struct http_request myreq;

int main(int argc, char* argv[]) 
{
	signal(SIGPIPE, SIG_IGN);
	SCREEN(SCREEN_BLUE, stdout, "This product is under development for the moment, please wait for a while.\nThank you for your support.\n\n");

	init_http_request(&myreq);
	parse_opt(argc, argv, &myreq);
	compose_request_buffer(&myreq);

	int poller_fd = ev_create();
	start_connection(poller_fd, &myreq);
	ev_run_loop(poller_fd, 100, myreq.ip, myreq.port);
	free_request_buffer(&myreq);

	return 0;
}
