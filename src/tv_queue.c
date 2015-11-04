#include <stdlib.h>

#include "tv_queue.h"
#include "screen.h"
#include "exception.h"

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
	queue->size	= 0;
}
