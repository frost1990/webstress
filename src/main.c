#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "request.h"
#include "caculate.h"
#include "screen.h"
#include "networking.h"
#include "sknet.h"
#include "hash_conn.h"
#include "ev.h"

long long request_seq = 0;
long long response_seq = 0;
struct timeval start;

struct http_request myreq;
extern hash_conn_t ghash_conn;

int main(int argc, char* argv[]) 
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, interupt_summary);
	SCREEN(SCREEN_GREEN, stdout, "Webstress - A simple web benchmark tool\nCopyright (c) 2015 frost, simple1115@gmail.com\n\n");
	SCREEN(SCREEN_BLUE, stdout, "This product is under development for the moment, please wait for a while.\nThank you for your support.\n\n");

	init_http_request(&myreq);
	parse_opt(argc, argv, &myreq);
	compose_request_buffer(&myreq);

	int poller_fd = ev_create();
	gettimeofday(&start, NULL);
	start_connection(poller_fd, &myreq);
	ev_run_loop(poller_fd, 100, myreq.ip, myreq.port);

	free_request_buffer(&myreq);
	hash_conn_free(&ghash_conn);

	return 0;
}
