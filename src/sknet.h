#ifndef _SK_NET_H_
#define _SK_NET_H_

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> /* TCP_NODELAY */
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <netdb.h> 
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

/* Only works in block mode */
int sk_set_rcv_timeout(int fd, int tv_sec, int tv_usec);
/* Only works in block mode */
int sk_set_snd_timeout(int fd, int tv_sec, int tv_usec);

int sk_set_rcv_buffer(int fd, size_t size);
int sk_set_snd_buffer(int fd, size_t size);
int sk_get_rcv_buffer(int fd);
int sk_get_snd_buffer(int fd);

int sk_shutdown_all(int fd);
int sk_shutdown_read(int fd);
int sk_shutdown_write(int fd);

int sk_getsockname(int fd, char *ip, size_t ip_len, int *port);
int sk_getpeername(int fd, char *ip, size_t ip_len, int *port);

/* Set nonblock I/O */
int sk_set_nonblock(int fd);
/* Set block I/O */
int sk_set_block(int fd);
/* Async connect */
int sk_async_connect(int poller_fd, int fd, const char *ip, int port);

int sk_async_ipv4_connect(int poller_fd, int fd, uint32_t ip, int port);

/* Turn off Nagle's algorithm */
int sk_tcp_no_delay(int fd);
/* Address reuse */ 
int sk_resue_addr(int fd);
/* Enable TCP Keep-Alive */
int sk_tcp_keepalive(int fd);

/* Check pending error for the socket */
int sk_check_so_error(int fd);

int sk_close(int fd);
/* Make sure drain all remaining data from tcp recv buffer and send A FIN segment instead of RST */
/* Please make sure your socket file descriptor working in nonblock mode */
int sk_fin_close(int fd);

/* Get hosts or dns query */
uint32_t sk_get_host_ipv4(char *host);

/* Switch unsigned ip to dotted decimal string */
void sk_ipv4_tostr(uint32_t ip, char *ipstr, size_t ipstr_len);

/* Switch dotted decimal string to unsigned int */
uint32_t sk_str_to_ipv4(const char *ipstr);

int sk_bind_listen(int listen_fd, int port, int backlog); 

#endif 
