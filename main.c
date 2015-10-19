#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
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

int main(int argc, char* argv[]) {

	return 0;
}
