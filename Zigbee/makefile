##############################################################################
CC=gcc
CFLAGS=-std=c99 -U__STRICT_ANSI__ \
       -W -Wall -pedantic -O3 \
       -D_REENTRANT
LDFLAGS=-lpthread -lm
SOURCES=Serial_Init.c APIframe_gen.c Zigbee.c
EXECUTABLE=zigbee.bin


all: clean ${EXECUTABLE} 

.SUFFIXES:


${EXECUTABLE}:
	${CC} -o $@ ${SOURCES} ${CFLAGS} ${LDFLAGS}
	@echo


clean :
	rm -f ${EXECUTABLE} *.o a.out core

##############################################################################

