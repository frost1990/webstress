#include <errno.h>
#include <stdio.h>
#ifdef __linux__
	#include <sys/epoll.h>
#else 
	#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
		#include <sys/event.h>
	#else
		#include <sys/select.h>
	#endif
#endif

#include "ev.h"
#include "timer.h"

#ifdef __linux__
#include <sys/epoll.h>

/* Create kernel event table */
int ev_create(void)
{
	return epoll_create(5);
}

/* Register a new socket's readable event to poller, edge_trigger */
int ev_add_event(int poller_fd, int fd) 
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; 
	return epoll_ctl(poller_fd, EPOLL_CTL_ADD, fd, &event);
}

/* Modify a socket's event type to poller */
int ev_modify_event(int poller_fd, int fd, int active_type) 
{
	struct epoll_event event;
	event.data.fd = fd;
	if (active_type == EPOLLIN) {
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; 
	} else if (active_type == EPOLLOUT) {
		event.events = EPOLLIN | EPOLLET | EPOLLOUT | EPOLLRDHUP; 
	}
	return epoll_ctl(poller_fd, EPOLL_CTL_MOD, fd, &event);
}

/* Delete a registered socket from poller */ 
int ev_del_event(int poller_fd, int fd) {
	return	epoll_ctl(poller_fd, EPOLL_CTL_DEL, fd, NULL);
}

/* Create a timer fd to poller */
int ev_add_timer(int poller_fd, int timerfd, int sec, int msec, bool once_only, 
	int (*callback) (void *), void *args) 
{
	timer_set_interval(timerfd, sec, msec, once_only);

	struct epoll_event event;
	event.data.fd = timerfd;
	event.events = EPOLLIN | EPOLLET; 

	return epoll_ctl(poller_fd, EPOLL_CTL_ADD, timerfd, &event);
} 

int ev_del_timer(int poller_fd, int timerfd) {
	return epoll_ctl(poller_fd, EPOLL_CTL_DEL, timerfd, NULL);
}

/* Start a event loop */
void ev_run_loop(int poller_fd, int timeout_usec) {
	struct epoll_event events[MAX_EVENT_NO];
	while (true) {
		int number = epoll_wait(poller_fd, events, MAX_EVENT_NO, timeout_usec); 
		if (number < 0 ) {
			if (errno == EINTR) {
				continue;
			}
			break;	
		}

		for (int i = 0; i < number; i++) {
			int event_fd = events[i].data.fd;
			/* event_fd may  be a socket or a timerfd */
		
		}
	}
	return;
}

#else
	#if (defined(__APPLE__)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
		#include <sys/event.h>

/* Create kernel event table */
int ev_create(void)
{
	return epoll_create(5);
}

/* Register a new socket's readable event to poller, edge_trigger */
int ev_add_event(int poller_fd, int fd) 
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; 
	return epoll_ctl(poller_fd, EPOLL_CTL_ADD, fd, &event);
}

/* Modify a socket's event type to poller */
int ev_modify_event(int poller_fd, int fd, int active_type) 
{
	struct epoll_event event;
	event.data.fd = fd;
	if (active_type == EPOLLIN) {
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; 
	} else if (active_type == EPOLLOUT) {
		event.events = EPOLLIN | EPOLLET | EPOLLOUT | EPOLLRDHUP; 
	}
	return epoll_ctl(poller_fd, EPOLL_CTL_MOD, fd, &event);
}

/* Delete a registered socket from poller */ 
int ev_del_event(int poller_fd, int fd) {
	return	epoll_ctl(poller_fd, EPOLL_CTL_DEL, fd, NULL);
}

/* Create a timer fd to poller */
int ev_add_timer(int poller_fd, int timerfd, int sec, int msec, bool once_only, 
	int (*callback) (void *), void *args) 
{
	timer_set_interval(timerfd, sec, msec, once_only);

	struct epoll_event event;
	event.data.fd = timerfd;
	event.events = EPOLLIN | EPOLLET; 

	return epoll_ctl(poller_fd, EPOLL_CTL_ADD, timerfd, &event);
} 

int ev_del_timer(int poller_fd, int timerfd) {
	return epoll_ctl(poller_fd, EPOLL_CTL_DEL, timerfd, NULL);
}

/* Start a event loop */
void ev_run_loop(int poller_fd, int timeout_usec) {
	struct epoll_event events[MAX_EVENT_NO];
	while (true) {
		int number = epoll_wait(poller_fd, events, MAX_EVENT_NO, timeout_usec); 
		if (number < 0 ) {
			if (errno == EINTR) {
				continue;
			}
			break;	
		}

		for (int i = 0; i < number; i++) {
			int event_fd = events[i].data.fd;
			/* event_fd may  be a socket or a timerfd */
		
		}
	}
	return;
}

#endif
