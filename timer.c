#include <unistd.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

/* Enable the timer and set interval, timer will first run after it's specified interval */
int timer_set_interval(int fd, int sec, int msec, bool once_run) {
	struct itimerspec new_time; 		
	struct timespec current;

	clock_gettime(CLOCK_REALTIME, &current);	

	new_time.it_value.tv_sec = current.tv_sec + sec;
	new_time.it_value.tv_nsec = current.tv_nsec + MILLION * msec;

	/* If nanosecond Field exceeds 1000000000, second field increases */
	if (new_time.it_value.tv_nsec >= BILLION) {
		new_time.it_value.tv_sec++;
		new_time.it_value.tv_nsec -= BILLION;
	}

	if (once_run) {
		new_time.it_interval.tv_sec = 0;
		new_time.it_interval.tv_nsec = 0;

	} else {
		new_time.it_interval.tv_sec = sec;
		new_time.it_interval.tv_nsec = 1000000 * msec;
	}

	return timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_time, NULL);
}


/* Turn off the timer (may be temporarily) */
int timer_disarm(int fd) {
	struct itimerspec new_time = { {0, 0}, {0, 0} };		
	return timerfd_settime(fd, 0, &new_time, NULL);
}
