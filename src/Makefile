uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
CC = gcc
CTAGS = ctags

INCLUDES = -I/usr/local/include -I/usr/include 
LIBS = -L/usr/local/lib  -L/usr/local/lib 
ifeq ($(uname_S),Linux)
	LINK = -lrt -lm -lprofiler -lunwind
endif
CFLAGS = -g -std=gnu99 -Wall -fno-strict-aliasing  -c $(INCLUDES) 
OFLAGS = $(LIBS) $(LINK)

SRCDIR = .
SRCS = $(wildcard $(SRCDIR)/*.cc $(SRCDIR)/*.c)
OBJS = $(addsuffix .o, $(basename $(SRCS)))
HEADS = $(addsuffix .h, $(basename $(SRCS)))
TARGET = webstress 

INSTALL_PATH = ../bin

APP = $(TARGET)

all : $(APP) install

show :
	@echo $(CFLAGS)

$(APP) : $(OBJS)
	$(CC) $^ -o $@ $(OFLAGS)

%.o:%.cc
	$(CC) $(CFLAGS) -o $@ $<

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $<

clean :
	rm -Rf *.o core.*
install:
	mkdir -p $(INSTALL_PATH); install $(TARGET) $(INSTALL_PATH)
