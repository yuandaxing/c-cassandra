AS      =as
LD      =ld
CC      =gcc
AR      =ar



CFLAGS = -g  -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -I../debug/ -I../serialize/ -I../mem/
CFLAGS += 
LDFLAGS = -L.   -lm -lpthread
SRCS = log.c  test.c
OBJS := $(SRCS:.c=.o)

EXECUT_TAGS := test

all:$(EXECUT_TAGS)


${EXECUT_TAGS}:${OBJS}
	ar crv libdebug.a $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o 
	$(RM) ${EXECUT_TAGS}
