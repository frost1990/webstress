#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define RECV_BUFFER_SIZE (64 * 1024)

long long request_seq = 0;
long long response_seq = 0;
struct timeval start;
struct timeval end;

const char *request = "GET /hello?body={\"to\":\"http://baidu.com\",\"action\":\"to\"} HTTP/1.1\r\n"
					  "Connection: keep-alive\r\n";
// Ip to dotted decimal string
char* iptostr(unsigned host_ip) {
	struct in_addr  addr;
	memcpy(&addr, &host_ip, 4); 
	return inet_ntoa(addr);
}


void interupt_stats(int signal) {
	gettimeofday(&end, NULL);
	uint32_t cost_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    fprintf(stderr, "\nProcess interupted by %s, total duration:%d seconds\n", strsignal(signal), cost_time / 1000);
    fprintf(stderr, "Total requests sent:%llu\n", request_seq);
    fprintf(stderr, "Total responses recieved:%llu\n", response_seq);
	double finished = (request_seq) ? (100 * (double)response_seq / (double)request_seq) : 0.0;
    fprintf(stderr, "Finished tasks percent %f%%\n", finished);
    exit(EXIT_FAILURE);
}

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void setaddrresue(int sockfd) {
	int resue = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &resue, sizeof(resue)) != 0) {
		printf("Set address resue failed, errno: %d, errStr: %s", errno, strerror(errno));
	}
}

void addfd(int epoll_fd, int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

bool check_error(int fd) {
	int error = 0;
	socklen_t length = sizeof(error);
	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &length) < 0) {
		printf("Get socket option failed\n");
		return false;
	}

	if (error != 0) {
		printf("Connection failed after epoll with the error: %d \n", error);
		return false;
	}
	return true;
}

bool send_request(int sockfd, const char* buffer, int len) {
	int bytes_write = 0;
	while (true) {   
		bytes_write = send(sockfd, buffer, len, 0);
		if (bytes_write < 0) {   
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
					// Not enough space in socket send buffer
				return false;
			} else if (errno == ECONNRESET) { // Recieve an RST segement from peer 
				return false;
			} else if (errno == EINTR) { // Interupted by a signal
				continue;
			}
		} else if (bytes_write == 0) {   
			return false;
		}   

		len -= bytes_write;
		buffer = buffer + bytes_write;
		if (len <= 0) {   
			request_seq++;
			return true;
		}   
	}   
}

bool recieve_response(int sockfd, char* buffer, int length) {
	int len = 0;		
	int bytes = 0;
	while (true) {
		len = recv(sockfd, buffer + bytes, length, 0);
		if (len < 0)  {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else if (errno == ECONNRESET) { 	
				// Recieve a RST segement from peer 
				printf("errno: %d, errstr: %s", errno, strerror(errno));
				return false;
			} else if (errno == EINTR) {	
				// Interupted by a signal
				printf("errno: %d, errstr: %s", errno, strerror(errno));
				continue;
			} 	
			return false;
		} else if (len == 0) {	
			// Recieve a FIN segement from peer
			return false;
		} else {
			bytes += len;
		}
	}
	printf("Content:\n%s\n", buffer);
	memset(buffer, 0, RECV_BUFFER_SIZE);
	response_seq++;
	return true;
}

void start_conn(int epoll_fd, int num, const char* ip, int port) {
	gettimeofday(&start, NULL);
	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	for (int i = 0; i < num; i++) {
		int sockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			continue;
		}
		setaddrresue(sockfd);
		if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == 0) {
			struct sockaddr_in from;
			int socklen = sizeof(from);
			getsockname(sockfd, (struct sockaddr *)&from, (socklen_t *)&socklen);
			printf("Connection from %s:%d to %s:%d enstablished\n", iptostr(from.sin_addr.s_addr), from.sin_port, ip, port);
			addfd(epoll_fd, sockfd);
		}
	}
	printf("Launched %d connections, benchmark will soon start\n", num);
}

void close_conn(int epoll_fd, int sockfd) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, 0);
	struct sockaddr_in from;
	int socklen = sizeof(from);
	getsockname(sockfd, (struct sockaddr *)&from, (socklen_t *)&socklen);
	printf("Connection from %s:%d will be closed\n", iptostr(from.sin_addr.s_addr), from.sin_port);
	close(sockfd);
}

void reconnect(int epoll_fd, const char *ip, int port) {
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return;
	}
	setnonblocking(sockfd);
	setaddrresue(sockfd);
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);
	
	if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == 0) {
		struct sockaddr_in from;
		int socklen = sizeof(from);
		getsockname(sockfd, (struct sockaddr *)&from, (socklen_t *)&socklen);
		printf("Connection from %s:%d to %s:%d enstablished\n", iptostr(from.sin_addr.s_addr), from.sin_port, ip, port);
		addfd(epoll_fd, sockfd);
	} else if (errno == EINPROGRESS || errno == EAGAIN) {
		addfd(epoll_fd, sockfd);
	} else {
		printf("Reconnect failed, errno: %d, errstr: %s", errno, strerror(errno));
	}

	return;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage: %s ip port connections\n", argv[0]);
		exit(0);
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, interupt_stats);
	int epoll_fd = epoll_create(100);
	
	start_conn(epoll_fd, atoi(argv[3]), argv[1], atoi(argv[2]));
	printf("Request content:\n%s\n", request);
	struct epoll_event events[10000];
	char *buffer = (char *) malloc(RECV_BUFFER_SIZE * sizeof (char));
	while (true) {
		int fds = epoll_wait(epoll_fd, events, 10000, 2000);
		for (int i = 0; i < fds; i++) {   
			int sockfd = events[i].data.fd;
			if (events[i].events & EPOLLRDHUP) { 	// Recieve a FIN segement from peer   
				close_conn(epoll_fd, sockfd);
				reconnect(epoll_fd, argv[1], atoi(argv[2]));
			} else if (events[i].events & EPOLLIN) {   
				if (!recieve_response(sockfd, buffer, RECV_BUFFER_SIZE)) {
					close_conn(epoll_fd, sockfd);
					reconnect(epoll_fd, argv[1], atoi(argv[2]));
				}
				struct epoll_event event;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);
			} else if (events[i].events & EPOLLOUT) {
				if (!check_error(sockfd)) {
					close_conn(epoll_fd, sockfd);
				}
				if (!send_request(sockfd, request, strlen(request))) {
					close_conn(epoll_fd, sockfd);
					reconnect(epoll_fd, argv[1], atoi(argv[2]));
				}
				struct epoll_event event;
				event.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);
			} else if(events[i].events & EPOLLERR) {
				close_conn(epoll_fd, sockfd);
				reconnect(epoll_fd, argv[1], atoi(argv[2]));
			}
		}
	}

	free(buffer);
	return 0;
}
