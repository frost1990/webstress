#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "caculate.h"
#include "screen.h"

void stats_init(stats_t* record) 
{
	record->size = 0;
	record->capacity = STATS_INIT_SIZE;
	record->data = (uint64_t *) malloc(record->capacity * sizeof(uint64_t));
	if (record->data == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}

	memset(record->data, 0, record->capacity);
}

void stats_add(stats_t* record, uint64_t element)
{
	record->size++;
	/* Auto resize, just like std::vector in C++ STL */
	if (record->size >= record->capacity / 2) {
		stats_resize(record);
	}

	record->data[record->size] = element;
}

void stats_resize(stats_t* record) 
{
	record->capacity *= 2;
	record->data = (uint64_t *)realloc(record->data, (record->capacity) * sizeof(uint64_t));
	if (record->data == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}
}

void stats_free(stats_t* record) 
{
	free(record->data);
}	

uint64_t stats_avg(stats_t *record) 
{
	/* FIXME: here may overflow */
	uint64_t sum = 0;
	for (int i = 0; i < record->size; i++) {
		sum += (record->data)[i];
	}

	return (uint64_t) (sum / record->size);
}

uint64_t stats_max(stats_t *record) 
{
	uint64_t max = 0;
	for (int i = 0; i < record->size; i++) {
		if (max < (record->data)[i]) {
			max = (record->data)[i];
		}
	}
	return max;
}

uint64_t stats_min(stats_t * record)
{
	uint64_t min = INT_MAX;
	for (int i = 0; i < record->size; i++) {
		if (min > (record->data)[i]) {
			min = (record->data)[i];
		}
	}
	return min;
}
