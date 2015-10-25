#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "config.h"
#include "http_parser.h"
#include "sknet.h"
#include "help.h"
#include "version.h"
#include "url.h"

char* iptostr(unsigned host_ip) {
    struct in_addr  addr;
    memcpy(&addr, &host_ip, 4);
    return inet_ntoa(addr);
}

int parse_opt(int argc, char **argv, config *request) 
{
	if (argc == 1) {  
		help();
		return 0;
	}

  	struct http_parser_url us;
	char *src_url = argv[argc - 1];
  	
	parse_url(src_url, &us, request);

	uint32_t ip = sk_get_host_ipv4(request->host); 
	if (ip == 0) {
		printf("DNS query error\n");
	}
	request->ip = ip; 

	request->port = PORT_HTTP; 
	char ipstr[64] = {0};
	sk_ipv4_tostr(request->ip, ipstr, strlen(ipstr));
	printf("Get IP %s after DNS query\n", ipstr);
	int ch;                     
	while ((ch = getopt(argc, argv, "c:H:hv")) != -1) {
		switch(ch) {
			case 'c':
				request->connections = atoi(optarg);
				break;
			case 'H':
				request->additional_header = optarg;
				break;
			case 'h':
				help();
				break;
			case 'v':
				show_version();
				break;
			case '?':
				break;
			case ':':
				break;
		}
	}

	return 0;
}
