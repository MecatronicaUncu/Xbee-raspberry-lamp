##############################################################################
CC=gcc
CFLAGS=-std=c99 -U__STRICT_ANSI__ \
       -W -Wall -pedantic -O3 \
       -D_REENTRANT
LDFLAGS=-lpthread -lm
SOURCES=prog05_tcp_client.c
EXECUTABLE=prog05_tcp_client.bin

all: clean ${EXECUTABLE} 

.SUFFIXES:


${EXECUTABLE}:
	${CC} -o $@ ${SOURCES} ${CFLAGS} ${LDFLAGS}
	@echo


clean :
	rm -f ${EXECUTABLE} *.o a.out core

##############################################################################
