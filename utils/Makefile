AS      =as
LD      =ld
CC      =gcc
AR      =ar



CFLAGS = -g  -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -I../debug/ -I../serialize/ -I../mem/
CFLAGS += 
LDFLAGS = -L../debug/ -L../serialize/   -lm -lpthread -ldebug -lserialize -ldebug
SRCS =  auxilFile.c crc32.c dict.c 
OBJS := $(SRCS:.c=.o)

EXECUT_TAGS := test

all:$(EXECUT_TAGS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS) 
	ar -crv libutils.a $(OBJS)

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o  *.a
	$(RM) ${EXECUT_TAGS}
