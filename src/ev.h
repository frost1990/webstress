#ifndef _EVENT_POLLER_
#define _EVENT_POLLER_

#include <stdbool.h>
#include <stdint.h>

#define MAX_EVENT_NO 1024

#define EVENT_READ 1001

#define EVENT_WRITE 1002
/* Stream socket peer closed connection */
#define EVENT_RDHUP 1003

/* Create kernel event table */
int ev_create(void);
/* Register a new socket's readable event to poller */
int ev_add_event(int poller_fd, int fd);
/* Modify a socket's event type to poller */
int ev_modify_event(int poller_fd, int fd, int active_type);
/* Delete a registered socket from poller */ 
int ev_del_event(int poller_fd, int fd);
/* Register a timer event to poller */
int ev_add_timer(int poller_fd, int timerfd, int sec, int msec, bool once_only);
/* Delete a timer event from poller */
int ev_del_timer(int poller_fd, int timerfd);

int ev_check_so_error(int fd); 

/* Start event loop */
void ev_run_loop(int poller_fd, int timeout_usec, uint32_t ip, int port);
int ev_close(int poller_fd);

#endif
