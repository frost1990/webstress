#include <stdbool.h>
#include <stdlib.h>

#include "hash_conn.h"
#include "list_conn.h"
#include "screen.h"

bool is_prime(int num) 
{
	if (num <= 1) { 
		return false;
	}

   	for (int i = 2; i * i <= num; i++) {
		if (num % i == 0) return false;
	}

 	return true;
}

uint32_t get_nearest_prime(uint32_t base)
{
	uint32_t i = base;
	while (true) {
		if (!is_prime(i)) {
			i++;
		} else {
			return i;
		}
	}
}

int modhash(int key, unsigned base)
{
	return key % base;
}

void hash_conn_init(hash_conn_t *phash_conn, int conns)
{
	phash_conn->table_size = get_nearest_prime(conns);
	phash_conn->elenum = 0; 

	phash_conn->idx_ptr = (bucket_t **) malloc(sizeof(bucket_t *) * phash_conn->table_size);

	if (phash_conn->idx_ptr == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < phash_conn->table_size; i++) {
		(phash_conn->idx_ptr)[i] = malloc(sizeof(conn_t));
		if ((phash_conn->idx_ptr)[i] == NULL) {
			SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
			exit(EXIT_FAILURE);
		}

		(phash_conn->idx_ptr)[i]->depth = 0;
		(phash_conn->idx_ptr)[i]->key = 0;
		(phash_conn->idx_ptr)[i]->val = NULL;
	}
}

void hash_conn_add(hash_conn_t *phash_conn, int fd)
{
	int idx = modhash(fd, phash_conn->table_size);
	bucket_t *location = (phash_conn->idx_ptr)[idx];
	conn_t *pconn = (conn_t *) malloc(sizeof(conn_t));
	if (pconn == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}
	pconn->fd = fd;
	pconn->recv_buffer = (char *) malloc(RECV_BUFFER_SIZE);
	if (pconn->recv_buffer == NULL) {
		SCREEN(SCREEN_RED, stderr, "Cannot allocate memory, malloc(3) failed.\n");
		exit(EXIT_FAILURE);
	}

	location->depth++;
	if (location->depth != 0) {
		list_conn_add(location, pconn);
	} else {
		location->key = fd;
		location->val = pconn;
	}

	phash_conn->elenum++; 
}

void hash_conn_free(hash_conn_t *phash_conn)
{
	for (int i = 0; i < phash_conn->table_size; i++) {
		list_conn_free((phash_conn->idx_ptr)[i]);
	}

	/* Do not forget free phash_conn somewhere else, if phash_conn is created from dynamic memory allocation */
	free(phash_conn->idx_ptr);
}
