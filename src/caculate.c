#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "caculate.h"
#include "screen.h"	

static struct http_status_code_t status_code_map [] = {
	{100, "Continue"},
	{101, "Switching Protocols"},	
	{102, "Processing"},

	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},	
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},	
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},

	{300, "Multiple Choices"},	
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},	
	{304, "Not Modified"},	
	{305, "Use Proxy"},
	{306, "(reserviert)"},
	{307, "Temporary Redirect"},	
	{308, "Permanent Redirect"},

	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},	
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},	
	{408, "Request Time-out"},	
	{409, "Conflict"},	
	{410, "Gone"},
	{411, "Length Required"},	
	{412, "Precondition Failed"},	
	{413, "Request Entity Too Large"},	
	{414, "Request-URL Too Long"},	
	{415, "Unsupported Media Type"},	
	{416, "Requested"},
	{417, "Expectation Failed"},
	{418, "I’m a teapo"},
	{420, "Policy Not Fulfilled"},	
	{421, "Misdirected Request"},	
	{422, "Unprocessable Entity"},	
	{423, "Locked"},	
	{424, "Failed Dependency"},	
	{425, "Unordered Collection"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},	
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},	
	{444, "No Response"},
	{449, "The request should be retried after doing the appropriate action"},	
	{451, "Unavailable For Legal Reasons"},
	{499, "Client has closed connection"},

	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},	
	{503, "Service Unavailable"},	
	{504, "Gateway Time-out"},
	{505, "HTTP Version not supported"},	
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{509, "Bandwidth Limit Exceeded"},	
	{510, "Not Extended"},
};

extern struct timeval start;
extern struct http_request myreq;
extern stats_t net_record;
extern uint32_t g_status_code_map[1024];

static const char *get_status_code_info(uint32_t code) 
{
	for (int i = 0; i < sizeof(status_code_map) / sizeof(http_status_code_t); i++) {
		if (code == status_code_map[i].code) {
			return status_code_map[i].information;
		}
	}
	return NULL;
}

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
	record->data = (uint32_t *) malloc(record->capacity * sizeof(uint32_t));
	record->total_requests = 0;
	record->total_responses = 0; 
	record->snd_bytes = 0; 
	record->rcv_bytes = 0; 
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
	uint32_t *previous_buffer = record->data;
	record->data = (uint32_t *) realloc(previous_buffer, (record->capacity) * sizeof(uint32_t));
	if (record->data == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}
	memset(record->data + record->size + 1, 0, record->capacity - record->size);
}

void stats_free(stats_t* record) 
{
	free(record->data);
}	

double stats_avg(stats_t *record) 
{
	uint64_t sum = 0;
	for (int i = 0; i < record->size; i++) {
		sum += (record->data)[i];
	}

	return (sum / (double) record->size);
}

double stats_navg(stats_t *record, int n) 
{
	if (n >= record->size) {
		return stats_avg(record);
	}

	uint32_t sum = 0;
	for (int i = 0; i < n; i++) {
		sum += (record->data)[i];
	}

	return (sum / (double) n);
}

double stats_navg2(stats_t *record, int m, int n) 
{
	if (n > record->size ||  m > record->size) {
		return stats_avg(record);
	}

	if (m == n) {
		return (record->data)[m];
	}

	int max = (m > n) ? m: n;
	int min = (m < n) ? m: n;

	uint32_t sum = 0;
	for (int i = min + 1; i < max; i++) {
		sum += (record->data)[i];
	}

	return (sum / (double) (max - min));
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
	quicksort(record->data, 0, record->size - 1);
}

void stats_vector_debug_show(stats_t *record)
{
	for (int i = 0; i < record->size; i++) {
		SCREEN(SCREEN_YELLOW, stdout, "%u ", (record->data)[i]);
	}
	SCREEN(SCREEN_YELLOW, stdout, "\n");

}

void summary_status_code(uint32_t* array, int length) 
{
	uint32_t sum = 0;
	for (int i = 0; i < length; i++) {
		sum += array[i];
	}

	for (int i = 0; i < length; i++) {
		if (array[i] != 0) {
			const char *info = get_status_code_info(i);
			if (info != NULL) {	
				SCREEN(SCREEN_DARK_GREEN, stdout, "%d %s\t%4.2f\%%\n", i, info, 100 * (double)array[i] / (double) sum);
			} else {
				SCREEN(SCREEN_DARK_GREEN, stdout, "%d\t%4.2f\%%\n", i, 100 * (double)array[i] / (double) sum);
			}
		}
	}

	SCREEN(SCREEN_YELLOW, stdout, "\n");
}

/* Before call stats_show_percentage(), make sure your record is sorted */
void stats_show_percentage(stats_t *record)
{
	int top10 = record->size * 1 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 1.00 * 100 / 10, stats_navg2(record, 0, top10) / 1000);
	int top20 = record->size * 2 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 2.00 * 100/ 10, stats_navg2(record, top10, top20) / 1000);
	int top30 = record->size * 3 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 3.00 * 100/ 10, stats_navg2(record, top20, top30) / 1000);
	int top40 = record->size * 4 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 4.00 * 100 / 10, stats_navg2(record, top30, top40) / 1000);
	int top50 = record->size * 5 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 5.00 * 100 / 10, stats_navg2(record, top40, top50) / 1000);
	int top60 = record->size * 6 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 6.00 * 100 / 10, stats_navg2(record, top50, top60) / 1000);
	int top70 = record->size * 7 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 7.00 * 100 / 10, stats_navg2(record, top60, top70) / 1000);
	int top80 = record->size * 8 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 8.00 * 100 / 10, stats_navg2(record, top70, top80) / 1000);
	int top90 = record->size * 9 / 10;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 9.00 * 100 / 10, stats_navg2(record, top80, top90) / 1000);
	int top95 = record->size * 95 / 100;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 9.50 * 100 / 10, stats_navg2(record, top90, top95) / 1000);
	int top999 = record->size * 999 / 1000;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n", 9.99 * 100 / 10, stats_navg2(record, top95, top999) / 1000);
	int topall = record->size;
	SCREEN(SCREEN_DARK_GREEN, stdout, "%4.2f\%%\t\t\t%4.3f ms\n\n", 10.00 * 100 / 10, stats_navg2(record, top999, topall) / 1000);
}	

void stats_traffic(stats_t *record, double seconds)
{
	SCREEN(SCREEN_YELLOW, stdout, "Http Traffic stats:\n");
	uint64_t rx = record->rcv_bytes;
	uint64_t tx = record->snd_bytes;

	double tgb = rx / GB;
	double tmb = rx / MB;
	double tkb = rx / KB;
	if (tgb > 1.0) {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Recieved:\t%4.3fGB, %4.3fGB/sec\n", tgb, tgb / seconds);
	} else if (tmb > 1.0) {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Recieved:\t%4.3fMB, %4.3fMB/sec\n", tmb, tmb / seconds);
	} else {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Recieved:\t%4.3fKB, %4.3fKB/sec\n", tkb, tkb / seconds);
	}

	double rgb = tx / GB;
	double rmb = tx / MB;
	double rkb = tx / KB;
	if (rgb > 1.0) {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Sent:\t\t%4.3fGB, %4.3fGB/sec\n\n", rgb, rgb / seconds);
	} else if (rmb > 1.0) {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Sent:\t\t%4.3fMB, %4.3fMB/sec\n\n", rmb, rmb / seconds);
	} else {
		SCREEN(SCREEN_DARK_GREEN, stdout, "Sent:\t\t%4.3fKB, %4.3fKB/sec\n\n", rkb, rkb / seconds);
	}
}

void stats_summary(http_request *request, stats_t *record)
{
	struct timeval end;
	gettimeofday(&end, NULL);
	uint32_t duration = stats_get_interval(&start, &end);
	double seconds = ((long double)duration) / (1000000.00);

	SCREEN(SCREEN_WHITE, stdout, "========================================Benchmarking terminates now========================================\n");
	SCREEN(SCREEN_YELLOW, stdout, "Total summary:\n");
	SCREEN(SCREEN_DARK_GREEN, stdout, "Duration: %4.2f seconds\n", seconds);
	SCREEN(SCREEN_DARK_GREEN, stdout, "%lu requests sent, %4.2f requests/sec\n", record->total_requests, record->total_requests / seconds);
	SCREEN(SCREEN_DARK_GREEN, stdout, "%lu responses recieved, %4.2f responses/sec\n", record->total_responses, record->total_responses / seconds);
	double finished = (record->total_responses) ? ((double) record->total_responses/ (double) record->total_requests) : 0;
	SCREEN(SCREEN_DARK_GREEN, stdout, "Finished tasks percent: %4.2f\%%\n\n", 100 * finished);

	stats_traffic(record, seconds);

	if (record->total_responses > 0) {
		SCREEN(SCREEN_YELLOW, stdout, "Http status code brief:\n");
		summary_status_code(g_status_code_map, 1024);

		double avg = stats_avg(record);
		uint32_t max = stats_max(record);
		uint32_t min = stats_min(record);
		double stddev = stats_stddev(record);
		stats_sort(record);	
		uint32_t median = (record->data)[record->size / 2];
		SCREEN(SCREEN_YELLOW, stdout, "Cost time:\n");
		SCREEN(SCREEN_DARK_GREEN, stdout, "Average:\t\t%4.3f ms\n", avg / 1000);
		SCREEN(SCREEN_DARK_GREEN, stdout, "Median:\t\t\t%4.3f ms\n", (double) median / 1000);
		SCREEN(SCREEN_DARK_GREEN, stdout, "Maximum:\t\t%4.3f ms\n", (double) max / 1000.00);
		SCREEN(SCREEN_DARK_GREEN, stdout, "Mininum:\t\t%4.3f ms\n", (double) min / 1000.00);
		SCREEN(SCREEN_DARK_GREEN, stdout, "Standard deviation:\t%4.3f ms\n\n", stddev / 1000.00);
	}
	
	if (record->size >= 100) {
		SCREEN(SCREEN_YELLOW, stdout, "Cost time percentage distribution:\n");
		stats_show_percentage(record);	
	}

	stats_free(record);
	return;
}

void interupt_summary(int signal) 
{
	SCREEN(SCREEN_GREEN, stdout, "\nProgram interupted by %s\n", strsignal(signal));
	stats_summary(&myreq, &net_record);
	exit(EXIT_SUCCESS);
}
