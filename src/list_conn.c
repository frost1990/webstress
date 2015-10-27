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
	list->next->pre  = list;
	list->key = pconn->fd;
	list->val = pconn;
}

void list_conn_free(bucket_t *list)
{
	bucket_t *store = list;
	for ( ; list != NULL; ) {
		store = list;	
		list = list->next;	
		free(store);
	}
}
