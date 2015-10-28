#include "help.h"
#include "regex_util.h"
#include "sknet.h"
#include "ev.h"
#include "timer.h"
#include "url.h"
#include "request.h"
#include "http_parser.h"
#include "networking.h"
#include "screen.h"
#include "hash_conn.h"
#include "list_conn.h"
#include "version.h"
#ifdef __linux__
	#include <malloc.h>
#endif

int main() {
	hash_conn_t ht;
	hash_conn_init(&ht, 10);

	for (int i = 0; i < 60; i++) {
		hash_conn_add(&ht, i);
	}
	hash_conn_debug_show(&ht);
#ifdef __linux__
	malloc_stats();
#endif
	for (int i = 0; i < 30; i++) {
		hash_conn_delete(&ht, i);
	}
	hash_conn_debug_show(&ht);
#ifdef __linux__
	malloc_stats();
#endif
	hash_conn_free(&ht);
	return 0;
}
