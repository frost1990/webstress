#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "timer.h"
#include "exception.h"

#ifdef __linux__
#include <sys/timerfd.h>

int timer_init(timer_heap_t *timer, int capacity)
{
	timer->heap_array = calloc(1, capacity * sizeof(timer_node_t));
	ASSERT_ALLOCATE(timer->heap_array);
	timer->size = 0;
	timer->capacity = capacity;

	return 0;
}

int timer_free(timer_heap_t *timer)
{
	free(timer->heap_array); 
	timer->size = 0;
	timer->capacity = 0;
	return 0;
}

int timer_create_fd(void) 
{
	return timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
}

/* Enable the timer and set interval, timer will first run after it's specified interval */
int timer_set_interval(int fd, int sec, int msec, bool once_run) 
{
	struct itimerspec new_time; 		
	struct timespec current;

	clock_gettime(CLOCK_REALTIME, &current);	

	new_time.it_value.tv_sec = current.tv_sec + sec;
	new_time.it_value.tv_nsec = current.tv_nsec + MILLION * msec;

	/* If nanosecond field exceeds 1000000000, second field increases */
	if (new_time.it_value.tv_nsec >= BILLION) {
		new_time.it_value.tv_sec++;
		new_time.it_value.tv_nsec -= BILLION;
	}

	if (once_run) {
		new_time.it_interval.tv_sec = 0;
		new_time.it_interval.tv_nsec = 0;

	} else {
		new_time.it_interval.tv_sec = sec;
		new_time.it_interval.tv_nsec = MILLION * msec;
	}

	return timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_time, NULL);
}

/* Turn off the timer (may be temporarily) */
int timer_disarm(int fd) 
{
	struct itimerspec new_time = { {0, 0}, {0, 0} };		
	return timerfd_settime(fd, 0, &new_time, NULL);
}

int timer_close(int fd) 
{
	return close(fd);
}

#endif
