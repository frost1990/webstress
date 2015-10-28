#include "list_conn.h"

int list_conn_length(bucket_t *list) 
{
	if (list == NULL) {
		return 0;
	}
	int count = 0;
	for ( ; list != NULL; list = list->next) {
		count++;
	}
	return count;
}

void list_conn_add(bucket_t *list, conn_t *pconn)
{
	while (list->next != NULL) {
		list = list->next;
	}

	list->next = (bucket_t *) malloc(sizeof(bucket_t));
	if (list->next == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}

	list->next->next = NULL;
	list->key = pconn->fd;
	list->val = pconn;
	list->depth = 0;
}

void list_conn_delete(bucket_t *list, int fd)
{
	/* In this case, depth = 1 */
	if (list->key == fd) {
		/* Free connection recieve buffer (char *) */ 
		free(list->val->recv_buffer);
		/* Free conn_t */ 
		free(list->val);
		return;
	}

	bucket_t* p = list; 
	for ( ; p->next != NULL; p = p->next ) {
		bucket_t* q = p->next; 
		if (q->key == fd) {
			/* Free connection recieve buffer (char *) */ 
			free(q->val->recv_buffer);
			/* Free conn_t */ 
			free(q->val);
			p->next = q->next;
			/* Free bucket_t */ 
			free(q);
			return;
		}
	}
}

void list_conn_free(bucket_t *list)
{
	bucket_t *store = list;
	for ( ; list != NULL; ) {
		store = list;	
		list = list->next;	
		free_conn_rcv_buffer(store->val);
		free(store);
	}
}