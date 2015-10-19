#ifndef _SK_NET_H_
#define _SK_NET_H_

#define SK_OK 0
#define SK_ERR -1

/* Only used in block mode */
int sk_set_rcv_timeout(int fd, int tv_sec, int tv_usec)
/* Only used in block mode */
int sk_set_snd_timeout(int fd, int tv_sec, int tv_usec)

int sk_set_rcv_buffer(size_t size, int fd);
int sk_set_snd_buffer(size_t size, int fd);
int sk_get_rcv_buffer(int fd);
int sk_get_snd_buffer(int fd);

int sk_shutdown_all(int fd);
int sk_shutdown_read(int fd);
int sk_shutdown_write(int fd);

int sk_getsockname(int fd, char *ip, size_t ip_len, int *port);
int sk_getpeername(int fd, char *ip, size_t ip_len, int *port);

/* Set nonblock I/O */
int sk_set_nonblocking(int fd);
/* Turn off Nagle's algorithm */
int sk_tcp_no_delay(int fd);
/* Address reuse */ 
int sk_resue_addr(int fd);
/* Enable TCP Keep-Alive */
int sk_tcp_keepalive(int fd);

/* Get /etc/hosts or dns query */
uint32_t sk_get_host_ipv4(char *host)

#endif 