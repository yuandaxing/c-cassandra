AS      =as
LD      =ld
CC      =gcc
AR      =ar



CFLAGS = -g  -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -I../debug/ -I../serialize/ -I../mem/  -I../include/ -I../utils
CFLAGS += 
LDFLAGS = -L../debug -L../serialize  -L../utils -lm -lpthread -ldebug -lserialize  -lutils
SRCS =  config.c tableMetadata.c test.c
OBJS := $(SRCS:.c=.o)

EXECUT_TAGS := test

all:$(EXECUT_TAGS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS) 
	ar -crv libconfig.a $(OBJS)

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o  *.a
	$(RM) ${EXECUT_TAGS}
