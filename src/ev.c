#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include "ev.h"
#include "timer.h"
#include "networking.h"
#include "sknet.h"

/* Only support advanced I/O multiplex(i.e. epoll, kevent) now, select(2) and poll(2) are not available for the moment */
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
	if (active_type == EVENT_READ) {
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; 
	} else if (active_type == EVENT_WRITE) {
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
void ev_run_loop(int poller_fd, int timeout_usec, uint32_t ip, int port) {
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
		/* event_fd may  be a socket or a timerfd */
			int fd = events[i].data.fd;
			if (events[i].events & EPOLLRDHUP) {   
				close_connection(poller_fd, fd);
				reconnect(poller_fd, ip, port);
				continue;
			} else if (events[i].events & EPOLLIN) {   
				int	ret = recieve_response(poller_fd, fd);
				if (ret <= 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				} 
				
				ev_modify_event(poller_fd, fd, EVENT_WRITE); 
			} else if (events[i].events & EPOLLOUT) {
				if (sk_check_so_error(fd) != 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				}
				int ret = send_request(poller_fd, fd);
				if (ret <= 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				}
				ev_modify_event(poller_fd, fd, EVENT_READ); 
			} else if (events[i].events & EPOLLERR) {
				close_connection(poller_fd, fd);
				reconnect(poller_fd, ip, port);
				continue;
			}
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
	return kqueue();
}

/* Register a new socket's readable event to poller, edge_trigger */
int ev_add_event(int poller_fd, int fd) 
{
	struct kevent ke;
	EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	return kevent(poller_fd, &ke, 1, NULL, 0, NULL);
}

/* Modify a socket's event type to poller */
int ev_modify_event(int poller_fd, int fd, int active_type) 
{	
	struct kevent ke;
	if (active_type == EVENT_READ) {
		EV_SET(&ke, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
	} else if (active_type == EVENT_WRITE) {
		EV_SET(&ke, fd, EVFILT_WRITE | EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
	}
	return kevent(poller_fd, &ke, 1, NULL, 0, NULL);
}

/* Delete a registered socket from poller */ 
int ev_del_event(int poller_fd, int fd) 
{
	struct kevent ke;
	EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(poller_fd, &ke, 1, NULL, 0, NULL);
	EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(poller_fd, &ke, 1, NULL, 0, NULL);
	return 0;
}

/* Create a timer fd to poller */
int ev_add_timer(int poller_fd, int timerfd, int sec, int msec, bool once_only, 
	int (*callback) (void *), void *args) 
{
	return 0;	
} 

int ev_del_timer(int poller_fd, int timerfd) {
	return 0;	
}

/* Start a event loop */
void ev_run_loop(int poller_fd, int timeout_usec, uint32_t ip, int port) {
	struct kevent events[MAX_EVENT_NO];
	int nfds = 0;
	struct timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = timeout_usec * THOUSAND;

	while (true) {
		nfds = kevent(poller_fd, NULL, 0, events, MAX_EVENT_NO, &timeout);
		if (nfds < 0 ) {
			if (errno == EINTR) {
				continue;
			}
			break;	
		}

		for (int i = 0; i < nfds; i++) {
			int fd = events[i].ident;
			/* If detect an EOF */
			if (events[i].flags & EV_EOF) {   
				close_connection(poller_fd, fd);
				reconnect(poller_fd, ip, port);
				continue;
			}
			/* Report any error */
			if (events[i].flags & EV_ERROR) {   
				close_connection(poller_fd, fd);
				reconnect(poller_fd, ip, port);
				continue;
			}

			if (events[i].filter == EVFILT_READ) {
				int ret = recieve_response(poller_fd, fd);
				if (ret <= 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				}
				ev_modify_event(poller_fd, fd, EVENT_WRITE); 
			}

			if (events[i].filter == EVFILT_WRITE) {
				if (sk_check_so_error(fd) != 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				}
				int ret = send_request(poller_fd, fd);
				if (ret <= 0) {
					close_connection(poller_fd, fd);
					reconnect(poller_fd, ip, port);
					continue;
				}
				ev_modify_event(poller_fd, fd, EVENT_READ); 
			}
		}
	}
	return;
}

#endif
#endif
