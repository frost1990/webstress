#ifndef _CACULATE_H
#define _CACULATE_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "request.h"
#include "response.h"

#define STATS_INIT_SIZE 2048

/* Stores send request times, recieve response times, all request-response cost intervals */
typedef struct stats_t {
	size_t capacity;
	size_t size;
	uint64_t total_requests;
	uint64_t total_responses;
	uint32_t *data;
} stats_t;

void stats_init(stats_t* record);

void stats_add(stats_t* record, uint32_t element);

void stats_resize(stats_t* record);

void stats_free(stats_t* record);

uint32_t stats_get_interval(struct timeval *start, struct timeval *end); 

double stats_avg(stats_t *record);

uint32_t stats_max(stats_t *record);

uint32_t stats_min(stats_t * record);

double stats_stddev(stats_t * record);

void stats_sort(stats_t *record);

void stats_summary(http_request *request, stats_t *record);

void interupt_summary(int signal);

#endif 
