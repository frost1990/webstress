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

struct http_request request;

int main(int argc, char* argv[]) 
{
	signal(SIGPIPE, SIG_IGN);
	SCREEN(SCREEN_BLUE, stdout, "This product is under development for the moment, please wait for a while.\nThank you for your support.\n\n");

	init_http_request(&request);
	parse_opt(argc, argv, &request);
	compose_request_buffer(&request);

	int poller_fd = ev_create();
	start_connection(poller_fd, &request);
	
//	ev_run_loop(poller_fd, 100);

	free_request_buffer(&request);

	return 0;
}
