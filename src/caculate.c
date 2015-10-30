#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "caculate.h"
#include "screen.h"	

extern struct timeval start;
extern struct http_request myreq;
extern stats_t net_record;

static void quicksort(uint32_t *array, int left, int right) {
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

uint32_t stats_get_interval(struct timeval *start, struct timeval *end) 
{
	uint32_t interval = end->tv_sec * 1000 * 1000 + end->tv_usec - start->tv_sec * 1000 * 1000 - start->tv_usec;
	return interval;
}

void stats_init(stats_t* record) 
{
	record->size = 0;
	record->capacity = STATS_INIT_SIZE;
	printf("hello: %d %s\n", __LINE__, __FILE__);
	record->data = (uint32_t *) malloc(record->capacity * sizeof(uint32_t));
	record->total_requests = 0;
	record->total_responses = 0; 
	if (record->data == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}

	memset(record->data, 0, record->capacity);
}

void stats_add(stats_t* record, uint32_t element)
{
	/* Auto resize, just like std::vector in C++ STL */
	if (record->size >= record->capacity / 2) {
		stats_resize(record);
	}

	record->data[record->size] = element;
	record->size++;
}

void stats_resize(stats_t* record) 
{
	record->capacity *= 2;
	printf("hello: %d %s\n", __LINE__, __FILE__);
	record->data = (uint32_t *)realloc(record->data, (record->capacity) * sizeof(uint32_t));
	if (record->data == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}
	memset(record->data, 0, record->capacity - record->size);
}

void stats_free(stats_t* record) 
{
	free(record->data);
}	

double stats_avg(stats_t *record) 
{
	/* FIXME: here may overflow */
	uint32_t sum = 0;
	for (int i = 0; i < record->size; i++) {
		sum += (record->data)[i];
	}

	return (sum / (long double) record->size);
}

uint32_t stats_max(stats_t *record) 
{
	uint32_t max = 0;
	for (int i = 0; i < record->size; i++) {
		if (max < (record->data)[i]) {
			max = (record->data)[i];
		}
	}
	return max;
}

uint32_t stats_min(stats_t * record)
{
	uint32_t min = INT_MAX - 1;
	for (int i = 0; i < record->size; i++) {
		if (min > (record->data)[i] ) {
			min = (record->data)[i];
		}
	}
	return min;
}

double stats_stddev(stats_t * record)
{
	double sum = 0.0;
	double avg = stats_avg(record);
	for (int i = 0; i < record->size; i++) {
		double element = (long double)record->data[i];
		sum = + powl(element - avg, 2);
	}

	return sqrtl(sum / (record->size));
}

void stats_sort(stats_t *record) 
{
	quicksort(record->data, 0, record->size - 1);
}

void stats_vector_debug_show(stats_t *record)
{
	for (int i = 0; i < record->size; i++) {
		SCREEN(SCREEN_YELLOW, stdout, "%u ", (record->data)[i]);
	}
	SCREEN(SCREEN_YELLOW, stdout, "\n");

}

void stats_summary(http_request *request, stats_t *record)
{
	struct timeval end;
	gettimeofday(&end, NULL);
	uint32_t duration = stats_get_interval(&start, &end);
	double seconds = ((long double)duration) / (1000000.00);

	SCREEN(SCREEN_WHITE, stdout, "=============================Benchmarking terminates now=============================\n\n");
	SCREEN(SCREEN_YELLOW, stdout, "Total summary\n");
	SCREEN(SCREEN_DARK_GREEN, stdout, "Duration: %4.2f seconds\n", seconds);
	SCREEN(SCREEN_DARK_GREEN, stdout, "%lu requests sent, %4.2f requests per second\n", record->total_requests, record->total_requests / seconds);
	SCREEN(SCREEN_DARK_GREEN, stdout, "%lu responses recieved, %4.2f responses per second\n", record->total_responses, record->total_responses / seconds);
	double finished = (record->total_responses) ? ((double) record->total_responses/ (double) record->total_requests) : 0;
	SCREEN(SCREEN_DARK_GREEN, stdout, "Finished tasks percent: %4.2f\%%\n\n", 100 * finished);

	double avg = stats_avg(record);
	uint32_t max = stats_max(record);
	uint32_t min = stats_min(record);
	double stddev = stats_stddev(record);

	SCREEN(SCREEN_YELLOW, stdout, "Cost time\n");
	SCREEN(SCREEN_DARK_GREEN, stdout, "Average: %4.3f ms\n", avg / 1000);
	SCREEN(SCREEN_DARK_GREEN, stdout, "Maximum: %4.3f ms\n", (double) max / 1000.00);
	SCREEN(SCREEN_DARK_GREEN, stdout, "Mininum: %4.3f ms\n", (double) min / 1000.00);
	SCREEN(SCREEN_DARK_GREEN, stdout, "Standard Deviation: %4.3f ms\n\n", stddev / 1000.00);
	
	SCREEN(SCREEN_YELLOW, stdout, "Percentage of the requests served within a certain time (ms)\n\n");

	SCREEN(SCREEN_YELLOW, stdout, "Http status code brief\n\n");

//	stats_vector_debug_show(record);

	stats_sort(record);	
	stats_free(record);
	return;
}

void interupt_summary(int signal) 
{
    SCREEN(SCREEN_GREEN, stdout, "\nProgram interupted by %s\n", strsignal(signal));
	stats_summary(&myreq, &net_record);
	exit(EXIT_SUCCESS);
}
