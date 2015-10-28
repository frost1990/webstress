#ifndef _LIST_CONNECTION_
#define _LIST_CONNECTION_

#include <stdlib.h>

#include "screen.h"
#include "networking.h"
#include "hash_conn.h"

int list_conn_length(bucket_t *list);

void list_conn_add(bucket_t *list, conn_t *pconn);

void list_conn_delete(bucket_t *list, int fd);

void list_conn_debug_show(bucket_t *list);

void list_conn_free(bucket_t *list);

#endif
