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
		(phash_conn->idx_ptr)[i]->key = -1;
		(phash_conn->idx_ptr)[i]->val = NULL;
		(phash_conn->idx_ptr)[i]->next = NULL;
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

	if (location->depth != 0) {
		list_conn_add(location, pconn);
	} else {
		location->key = fd;
		location->val = pconn;
	}

	/* Only the first element of the list's depth field is meaningful */
	location->depth++;
	phash_conn->elenum++; 
}

conn_t *hash_conn_get(hash_conn_t *phash_conn, int fd)
{

	int idx = modhash(fd, phash_conn->table_size);
	bucket_t *location = (phash_conn->idx_ptr)[idx];
	for (bucket_t * p = location; p != NULL; p = p->next) {
		if (p->key == fd) {
			return p->val;
		}
	}
	return NULL;
}

void hash_conn_delete(hash_conn_t *phash_conn, int fd)
{
	int idx = modhash(fd, phash_conn->table_size);
	bucket_t *location = (phash_conn->idx_ptr)[idx];
	if (location->depth == 0) {
		return;
	}	
	list_conn_delete(location, fd);
	location->depth--;
	phash_conn->elenum--;
	return;
}

void hash_conn_debug_show(hash_conn_t *phash_conn) 
{
	SCREEN(SCREEN_DARK_GREEN, stdout, "Hash table size %u, now %d elements\n", phash_conn->table_size, phash_conn->elenum);
	for (int i = 0; i < phash_conn->table_size; i++) {
		bucket_t *location = (phash_conn->idx_ptr)[i];
		int depth = location->depth; 
		if (depth == 0) {
			SCREEN(SCREEN_GREEN, stdout, "Slot %d is empty\n", i);
		} else {
			SCREEN(SCREEN_GREEN, stdout, "Slot %d depth is %d\n", i, location->depth);
			list_conn_debug_show(location);
		}
	}

}

void hash_conn_free(hash_conn_t *phash_conn)
{
	for (int i = 0; i < phash_conn->table_size; i++) {
		list_conn_free((phash_conn->idx_ptr)[i]);
	}

	/* Do not forget to free phash_conn somewhere else, if phash_conn is created from heap dynamic memory allocation */
	free(phash_conn->idx_ptr);
}
