#ifndef _CACULATE_H
#define _CACULATE_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "request.h"

#define STATS_INIT_SIZE (1024)
#define GB (1024.00 * 1024.00 * 1024.00)
#define MB (1024.00 * 1024.00)
#define KB (1024.00)

typedef struct http_status_code_t {
	uint32_t  code;
	char information[256];
} http_status_code_t;

/* Stores send request times, recieve response times, all request-response cost intervals */
typedef struct stats_t {
	size_t capacity;
	size_t size;
	double avg;
	uint64_t total_requests;
	uint64_t total_responses;
	uint64_t snd_bytes;
	uint64_t rcv_bytes;
	uint32_t *data;
	uint32_t fin_times;
	uint32_t rst_times;
	uint32_t max;
	uint32_t min;
	uint32_t dev;
	uint32_t timeout_times;
} stats_t;

void stats_init(stats_t* record);

void stats_add(stats_t* record, uint32_t element);

void stats_update(stats_t* record, uint32_t element);

void stats_resize(stats_t* record);

void stats_free(stats_t* record);

uint32_t stats_get_interval(struct timeval *start, struct timeval *end); 

double stats_avg(stats_t *record);

double stats_navg(stats_t *record, int n); 

double stats_navg2(stats_t *record, int m, int n); 

uint32_t stats_max(stats_t *record);

uint32_t stats_min(stats_t * record);

double stats_stddev(stats_t * record);

void stats_sort(stats_t *record);

/* Before call stats_show_percentage(), make sure your record is sorted */
void stats_show_percentage(stats_t *record);	

void stats_vector_debug_show(stats_t *record);

void stats_summary(http_request *request, stats_t *record);

void stats_traffic(stats_t *record, double seconds);

void interupt_summary(int signal);

#endif 
