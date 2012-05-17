#ifndef _BUFFER_H_
#define _BUFFER_H_
#include "string.h"
#include "stdio.h"
#include "log.h"
#include "stdlib.h"


typedef struct buffer{
    char *buff;
    int size;
    int pos;
}buffer;

buffer *getBuffer();
/* 0表示reserv成功，-1表示失败*/
int reserve(buffer *buff, int len); 
/*释放当前buffer*/ 
void freeHeapBuffer(buffer *buff);
int reset(buffer *buff);
int getBufferBytes(buffer *buff, char **bytes);
int getBytesCount(buffer *buff, int *count);
int bseekToPos(buffer *buff, int pos);
#endif
