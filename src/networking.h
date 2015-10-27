#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include "request.h"

int start_connection(int poller_fd, const http_request *request);

int recieve_response(int poller_fd, int fd);

int send_request(int poller_fd, int fd);

int close_connection(int poller_fd, int fd);

#endif
