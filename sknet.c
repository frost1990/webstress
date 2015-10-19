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

#include "sknet.h"

/* Only works in block mode */
int sk_set_rcv_timeout(int fd, int tv_sec, int tv_usec)
{
    struct timeval tv;
    tv.tv_sec = tv_sec;
    tv.tv_usec = tv_usec;

    return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

/* Only works in block mode */
int sk_set_snd_timeout(int fd, int tv_sec, int tv_usec)
{
	struct timeval tv;
	tv.tv_sec = tv_sec;
	tv.tv_usec = tv_usec;

    return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int sk_set_rcvbuffer(int fd, size_t size)
{
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, 4);
}

int sk_set_sndbuffer(int fd, size_t size) 
{
	return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, 4);
}

int sk_get_rcvbuffer(int fd) 
{
	int size;
	int	len = sizeof(size);
	int ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, (socklen_t*) &len);
	if (ret < 0) {
		return SK_ERR;
	}
	return size;
}

int sk_get_sndbuffer(int fd) 
{
	int size;
	int	len = sizeof(size);
	int ret = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, (socklen_t*) &len);
	if (ret < 0) {
		return SK_ERR;
	}
	return size;
}

int sk_shutdown_all(int fd) 
{
	return shutdown(fd, SHUT_RDWR);
}

int sk_shutdown_read(int fd) 
{
	return shutdown(fd, SHUT_RD);
}

int sk_shutdown_write(int fd) 
{
	return shutdown(fd, SHUT_WR);
}

int sk_getsockname(int fd, char *ip, size_t ip_len, int *port) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getsockname(fd, (struct sockaddr*)&sa, &salen) == -1) {
        if (port) *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET, (void*)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return 0;
}

int sk_getpeername(int fd, char *ip, size_t ip_len, int *port) 
{
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getpeername(fd, (struct sockaddr*)&sa, &salen) == -1) goto error;
    if (ip_len == 0) goto error;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET, (void*)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6, (void*)&(s->sin6_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin6_port);
    } else if (sa.ss_family == AF_UNIX) {
        if (ip) strncpy(ip, "/unixsocket", ip_len);
        if (port) *port = 0;
    } else {
        goto error;
    }
    return 0;

error:
    if (ip) {
        if (ip_len >= 2) {
            ip[0] = '?';
            ip[1] = '\0';
        } else if (ip_len == 1) {
            ip[0] = '\0';
        }
    }
    if (port) *port = 0;
    return SK_ERR;
}

/* Set nonblock I/O */
int sk_set_nonblock(int fd) 
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL)) != 0) {
		return SK_ERR;
	}
	return 0;
}

/* Set block I/O */
int sk_set_block(int fd)
{
	if (fcntl(fd, F_SETFL, ~O_NONBLOCK & fcntl(fd, F_GETFL)) != 0) {
		return SK_ERR;
	}
	return 0;
}

/* Turn off Nagle's algorithm */
int sk_tcp_no_delay(int fd)
{	
	int no_delay = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) != 0) {
		return SK_ERR;
	}
	return 0;
}

/* Address reuse */  
int sk_resue_addr(int fd) 
{
	int resue = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &resue, sizeof(resue)) != 0) {
		return SK_ERR;
	}
	return 0;
}

/* Enable TCP Keep-Alive */
int sk_tcp_keepalive(int fd) 
{
	int keep_alive = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) != 0) {
		return SK_ERR;
	}
	return 0;
}

int sk_close(int fd) 
{
	return close(fd);
}

/* Get hosts or dns query */
uint32_t sk_get_host_ipv4(char *host)
{
	uint32_t ip = 0;
	struct hostent *hosts;
	hosts = gethostbyname(host);

	if (hosts->h_addrtype == AF_INET) {
		if (hosts->h_addr_list[0]) {
			ip = ntohl((*(struct in_addr *)hosts->h_addr_list[0]).s_addr);
		}
	}
	return ip;
}

/* Switch unsigned ip to dotted decimal string */
void sk_ipv4_tostr(uint32_t ip, char *ipstr, size_t ipstr_len) 
{
	memset(ipstr, 0, ipstr_len);
	ip = ntohl(ip);
	size_t offset = 0;
	for (int i = 0; i < 4; i++) {
		char store[128];
		uint8_t segement = ip >> (8 * i);
		snprintf(store, sizeof(store) - 1, "%u", segement);
		size_t step = strlen(store);
		if (offset + step > ipstr_len - 1 ) {
			return;
		}
		offset += step;
		strncat(ipstr, store, step);
		if (i != 3) {
			size_t step_dot = strlen(".");
			if (offset + step_dot > ipstr_len - 1 ) {
				return;
			}
			strncat(ipstr, ".", step_dot);
			offset += strlen(".");
		}
	}
	return;
}
