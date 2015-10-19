#include <stdlib.h>
#include <stdio.h>

#include "version.h"

#define RECV_BUFFER_SIZE (64 * 1024)

long long request_seq = 0;
long long response_seq = 0;
struct timeval start;
struct timeval end;

const char *request = "GET /hello?body={\"to\":\"http://baidu.com\",\"action\":\"to\"} HTTP/1.1\r\n"
					  "Connection: keep-alive\r\n";

int main(int argc, char* argv[]) {
	printf("This product is under development for the momment, please wait for a while.\nThank you for your support!\n");

	return 0;
}
