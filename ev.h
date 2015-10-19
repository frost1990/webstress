#ifndef _EVENT_POLLER_
#define _EVENT_POLLER_

#define MAX_EVENT_NO 1024

typedef enum {
	LEVEL_TRIGGER = 10,
	EDGE_TRIGGER,
} Trigger_t;

/* Create kernel event table */
int ev_create(void);
/* Register a new socket's readable event to poller */
int ev_add_event(int poller_fd, int fd);
/* Modify a socket's event type to poller */
int ev_modify_event(int poller_fd, int fd, int active_type);
/* Delete a registered socket from poller */ 
int ev_del_event(int poller_fd, int fd);
/* Register a timer event to poller */
int ev_add_timer(int sec, int msec, bool once_only, int (*callback) (void *), void *args);
/* Delete a timer event from poller */
int ev_del_timer(int poller_fd, int timerfd);
/* Start event loop */
void ev_run_loop(int poller_fd, int timeout_usec);

#endif
