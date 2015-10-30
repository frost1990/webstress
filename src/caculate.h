#ifndef _CACULATE_H
#define _CACULATE_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#define STATS_INIT_SIZE 2048

typedef struct stats_t {
	uint64_t *data;
	size_t capacity;
	size_t size;
} stats_t;

void stats_init(stats_t* record);

void stats_add(stats_t* record, uint64_t element);

void stats_resize(stats_t* record);

void stats_free(stats_t* record);

uint64_t stats_avg(stats_t *record);

uint64_t stats_max(stats_t *record);

uint64_t stats_min(stats_t * record);

#endif 
