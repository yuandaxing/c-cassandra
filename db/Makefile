AS      =as
LD      =ld
CC      =gcc
AR      =ar



CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -I../debug/ -I../serialize/ -I../mem/ -I../utils/ -I../config -I../seda
CFLAGS += -I../include
LDFLAGS = -pg -lpthread -L../debug/ -L../serialize/ -L../config/  -L../utils/ -L../seda/ -lm -ldebug -lserialize -lconfig -lutils -lseda 
SRCS =   column.c superColumn.c columnFamily.c ssTable.c rowMutation.c memTable.c commitLog.c bloomFilter.c columnFamilyStore.c dataModel.c test.c indexHelper.c
OBJS := $(SRCS:.c=.o)

EXECUT_TAGS := test

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
