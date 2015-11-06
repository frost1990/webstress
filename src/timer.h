#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#define THOUSAND (1000lu) 
#define MILLION (1000000lu) 
#define BILLION (1000000000lu) 

#define TIMER_SIZE  1024 

typedef struct timer_node_t {
	struct timeval expire;
} timer_node_t;

typedef struct timer_heap_t {
	timer_node_t *heap_array;
	size_t size;
	size_t capacity;
} timer_heap_t;

int timer_init(timer_heap_t *timer, int capacity);

int timer_free(timer_heap_t *timer);

int timer_create_fd(void);
/* Enable the timer and set interval, timer will first run after it's specified interval */
int timer_set_interval(int fd, int sec, int msec, bool once_run);
/* Turn off the timer (may be temporarily) */
int timer_disarm(int fd);
int timer_close(int fd);

#endif 
