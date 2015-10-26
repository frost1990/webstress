#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>

#define THOUSAND (1000lu) 
#define MILLION (1000000lu) 
#define BILLION (1000000000lu) 

int timer_create_fd(void);
/* Enable the timer and set interval, timer will first run after it's specified interval */
int timer_set_interval(int fd, int sec, int msec, bool once_run);
/* Turn off the timer (may be temporarily) */
int timer_disarm(int fd);
int timer_close(int fd);

#endif 