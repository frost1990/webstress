#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "caculate.h"
#include "screen.h"	

static void quicksort(uint64_t *array, int left, int right) {
	if (left < right) {
		int key = array[left];
		/* Partition */
		size_t low = left;
		size_t high = right;
		while (low < high) {
			/* Low remain stable */
			while (low < high && array[high] >= key){
				high--;
			}
			if (high > low) {
				array[low] = array[high];
			}
			/* High remain stable */
			while (low < high && array[low] <= key){
				low++;
			}
			if (low < high) {
				array[high] = array[low];
			}
		}
		array[low] = key;
		/* Recursively invoke */
		quicksort(array, left, low - 1);
		quicksort(array, low + 1, right);
	} else {
		return;
	}
}

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

long double stats_avg(stats_t *record) 
{
	/* FIXME: here may overflow */
	uint64_t sum = 0;
	for (int i = 0; i < record->size; i++) {
		sum += (record->data)[i];
	}

	return (sum / (long double) record->size);
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

long double stats_stddev(stats_t * record)
{
	long double sum = 0.0;
	long double avg = stats_avg(record);
	for (int i = 0; i < record->size; i++) {
		long double element = (long double)record->data[i];
		sum = + powl(element - avg, 2);
	}

	return sqrtl(sum / (record->size));
}

void stats_sort(stats_t *record) 
{
	quicksort(record->data, 0, record->size);
}
