#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include "request.h"

int start_connection(int poller_fd, const http_request *request);

#endif
