#include <stdlib.h>
#include <stdio.h>

#include "config.h"

long long request_seq = 0;
long long response_seq = 0;
struct timeval start;
struct timeval end;

int main(int argc, char* argv[]) 
{
	printf("This product is under development for the moment, please wait for a while.\nThank you for your support!\n");

	parse_opt(argc, argv);
	
	return 0;
}
