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
	list->next->key = pconn->fd;
	list->next->val = pconn;
	list->next->depth = 0;
}

void list_conn_delete(bucket_t *list, int fd)
{
	/* In this case, delete the first node of the list  */
	if (list->key == fd) {
		/* Free connection recieve buffer (char *) */ 
		free_conn_rcv_buffer(list->val);
		/* Free conn_t */ 
		free(list->val);

		if (list->next != NULL) {
			bucket_t *store = list->next;
			list->key = store->key;
			list->val = store->val;
			list->next = store->next;
			/* Don't free store->val and store->val->recv_buffer */
			free(store);
		}
		return;
	}

	bucket_t* p = list; 
	for ( ; p->next != NULL; p = p->next ) {
		bucket_t* q = p->next; 
		if (q->key == fd) {
			/* Free connection recieve buffer (char *) */ 
			free_conn_rcv_buffer(q->val);
			/* Free conn_t */ 
			free(q->val);
			p->next = q->next;
			/* Free bucket_t */ 
			free(q);
			return;
		}
	}
}

void list_conn_debug_show(bucket_t *list)
{

	for ( ; list != NULL; list = list->next) {
		SCREEN(SCREEN_DARK_GREEN, stdout,"socket %d-> ", list->key);
	}
	SCREEN(SCREEN_DARK_GREEN, stdout,"NULL\n");
	return;
}

void list_conn_free(bucket_t *list)
{
	bucket_t *p = list;
	for ( ; p != NULL; ) {
		bucket_t *store = p;
		free_conn_rcv_buffer(store->val);
		p = store->next;
		/* Note: if p = list, will report "free(): invalid next size (fast)" because list is a continuous buffer in hash table slots 
 		this is not the situation when you free a two-dimensional dynamic array */
		if (store != list) {
			free(store);
		}
	}
}
