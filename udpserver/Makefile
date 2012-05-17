AS      =as
LD      =ld
CC      =gcc
AR      =ar



CFLAGS =  -g  -Wall -DMUTEX -DDEBUG -fno-strict-aliasing 
CFLAGS += 
LDFLAGS = -L.   -lm -lpthread -levent
SRCS =   log.c slist.c  udpsocket.c server.c
OBJS := $(SRCS:.c=.o)

EXECUT_TAGS := server

all:$(EXECUT_TAGS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS) 

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o 
	$(RM) ${EXECUT_TAGS}
