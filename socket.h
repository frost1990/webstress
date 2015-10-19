#ifndef _SOCKET_H_
#define _SOCKET_H_

#define SK_OK 0
#define SK_ERR -1

int sk_set_rcv_buffer(size_t size);
int sk_set_snd_buffer(size_t size);
int sk_get_rcv_buffer(int fd);
int sk_get_snd_buffer(int fd);
int sk_shutdown_all(int fd);
int sk_shutdown_read(int fd);
int sk_shutdown_write(int fd);
/* Set nonblock I/O */
int sk_set_nonblocking(int fd);
// Turn off Nagle's algorithm 
int sk_tcp_no_delay();
// Address reuse  
int sk_resue_addr();
// Enable TCP Keep-Alive
int sk_tcp_keepalive();

#endif 
