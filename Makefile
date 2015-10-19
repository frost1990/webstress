CC = gcc
CTAGS = ctags

INCLUDES = -I/usr/local/include -I/usr/include 
LIBS = -L/usr/local/lib -L/usr/lib -L../lib -L/usr/local/lib 
CFLAGS = -g -std=gnu99 -Wall -Wno-unused-result -O2 -c $(INCLUDES) 
OFLAGS = -g -Wall -O2 $(LIBS)

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
