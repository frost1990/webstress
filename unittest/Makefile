uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),Linux)
	LINK = -lrt 
endif

CFLAGS = -g -std=gnu99 -Wall -fno-strict-aliasing  
SRCS = 	../src/ev.c \
	   	../src/list_conn.c \
		../src/networking.c \
		../src/help.c \
		../src/hash_conn.c \
		../src/http_parser.c \
		../src/regex_util.c \
		../src/request.c \
		../src/screen.c \
		../src/sknet.c \
		../src/timer.c \
		../src/url.c \
		../src/version.c \
		unit.c 

unittester: 
	gcc -o unittester -Wall -I../src $(LINK) $(SRCS) $(CFLAGS) -g 
clean:
	rm -rf *.o ../src/*.o unittester
install:
