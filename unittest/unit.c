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

int main() {
	hash_conn_t ht;
	hash_conn_init(&ht, 10000);

	for(int i = 3; i < 20000; i++) {
		hash_conn_add(&ht, i);
	}

	return 0;
}
