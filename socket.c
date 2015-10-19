#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/tcp.h> /* TCP_NODELAY */
#include <fcntl.h> 
#include <netdb.h> 
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

int sk_set_rcv_buffer(int fd, size_t size)
{
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, 4);
}

int sk_set_snd_buffer(int fd, size_t size) 
{
	return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, 4);
}

int sk_get_rcv_buffer(int fd) 
{
	int size;
	int	len = sizeof(size);
	int ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, (socklen_t*) &len);
	if (ret < 0) {
		return SK_ERR;
	}
	return size;
}

int sk_get_snd_buffer(int fd) 
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
	return shutdown(sockfd_, SHUT_RDWR);
}

int sk_shutdown_read(int fd) 
{
	return shutdown(sockfd_, SHUT_RD);
}

int sk_shutdown_write(int fd) 
{
	return shutdown(sockfd_, SHUT_WR);
}

/* Set nonblock I/O */
int sk_set_nonblock(int fd) 
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL)) != 0) {
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
	if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &resue, sizeof(resue)) != 0) {
		return SK_ERR;
	}
	return 0;
}

/* Enable TCP Keep-Alive */
int sk_tcp_keepalive(int fd) 
{
	int keep_alive = 1;
	if (setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) != 0) {
		return SK_ERR;
	}
	return 0;
}
