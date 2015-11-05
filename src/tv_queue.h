#ifndef _TV_QUEUE_H
#define _TV_QUEUE_H

#include<time.h>
#include<sys/time.h>

typedef struct tv_node_t {
	struct timeval val;
	struct tv_node_t *next;
} tv_node_t;

typedef struct tv_queue_t {
	tv_node_t *head;
	tv_node_t *tail;
	size_t size;
} tv_queue_t;

void tv_init(tv_queue_t *queue);

void tv_pop(tv_queue_t *queue);

void tv_push(tv_queue_t *queue, struct timeval tv);

tv_node_t *tv_front(tv_queue_t *queue);

void tv_show_debug(tv_queue_t *queue);

void tv_free_all(tv_queue_t *queue);

#endif
