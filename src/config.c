#include <unistd.h>
#include <stdlib.h>

#include "config.h"
#include "help.h"
#include "version.h"

struct config request;

int parse_opt(int argc, char **argv) 
{
	if (argc == 1) {  
		help();
		return 0;
	}

	request.basic_url = argv[argc - 1];
	int ch;                     
	char error;
	char *b_opt_arg;            

	while ((ch = getopt(argc, argv, "c:H:hv")) != -1) {
		switch(ch) {
			case 'c':
				request.connections = atoi(optarg);
				break;
			case 'H':
				request.additional_header = optarg;
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
