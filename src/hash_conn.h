#ifndef _HASH_CONNECTION_H_
#define _HASH_CONNECTION_H_

#include <stdint.h>
#include <stdbool.h>

#include "networking.h"

typedef struct bucket_t {
	int key;				/* Socket file descriptor */
	conn_t *val;			/* Connection detail structure */
	int depth;				/* Depth of the bucket, alse means the length of conflict linked list */
	struct bucket_t *next; 	/* When conflict happens, use linked list to address latter elements */
	struct bucket_t *pre;	/* When conflict happens, use linked list to address latter elements */
} bucket_t;

typedef struct hash_map {
	uint32_t table_size;		/* Table size */
	int elenum;					/* Current element number */
	bucket_t **idx_ptr;
} hash_conn_t;

bool is_prime(int num); 

uint32_t get_nearest_prime(uint32_t base);

int modhash(int key, unsigned base);

void hash_conn_init(hash_conn_t *phash_conn, int conns);

void hash_conn_free(hash_conn_t *phash_conn);


#endif
