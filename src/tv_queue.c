#include <stdlib.h>

#include "tv_queue.h"
#include "screen.h"
#include "exception.h"

static void tv_show_time(struct timeval tv) 
{    
	struct tm ptv;
	localtime_r(&tv.tv_sec, &ptv);
	SCREEN(SCREEN_DARK_GREEN, stderr, "%02d:%02d:%02d %06d", ptv.tm_hour, ptv.tm_min, ptv.tm_sec, tv.tv_usec);
}	

void tv_init(tv_queue_t *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void tv_pop(tv_queue_t *queue) 
{
	tv_node_t *tmp = queue->head;
	if (tmp == NULL) {
		return;
	}

	queue->head = queue->head->next;
	queue->size--;
	free(tmp);
}

void tv_push(tv_queue_t *queue, struct timeval tv)
{
	tv_node_t *pnode = (tv_node_t *)malloc(sizeof(tv_node_t));
	ASSERT_ALLOCATE(pnode);

	pnode->val = tv;
	if (queue->size == 0) {
		queue->head = pnode;
		queue->tail = pnode;
	} else {
		queue->tail->next = pnode;
		queue->tail = pnode;
		queue->tail->next = NULL;
	}
	queue->size++;
}

tv_node_t *tv_front(tv_queue_t *queue)
{
	return queue->head;
}

tv_node_t *tv_end(tv_queue_t *queue)
{
	return queue->tail;
}

void tv_show_debug(tv_queue_t *queue)
{
	tv_node_t *p = queue->head;
	for ( ; p != NULL; p = p->next) {
		tv_show_time(p->val);
	}
	SCREEN(SCREEN_DARK_GREEN, stdout,"END\n");
	return;
}

void tv_free_all(tv_queue_t *queue)
{
	tv_node_t *p = queue->head;
	while (p != NULL) {
		tv_node_t* tmp = p;
		p = p->next;
		free(tmp);		
	}
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}
