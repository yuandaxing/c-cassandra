#include "buffer.h"

buffer *getBuffer()
{
	int size = 64;
	char *byte = (char *)malloc(size);
	buffer *buf = (buffer *)malloc(sizeof(buffer));
	
	if(!byte || !buf){
		if(byte) free(byte);
		if(buf)	 free(buf);
		return NULL;
	}
	
	buf->pos = 0;
	buf->size = size;
	buf->buff = byte;
	return buf;
}
int reserve(buffer *buff, int len)
{
	POINTER_NOT_NULL(buff);
	if(buff->size - buff->pos > len)
		return 0; 
	//这里选取的是一个固定值*/
	int newSize = buff->size + len + 64;
	char *byte = (char *)realloc(buff->buff, 
			newSize);

	if(!byte){
		return -1;
	}
	buff->size = newSize;
	buff->buff = byte;
	return 0;
}
void freeHeapBuffer(buffer *buff)
{
	if(!buff) return;

	if(buff->buff){
		free(buff->buff);
	}
	free(buff);
}
int reset(buffer *buff)
{
	POINTER_NOT_NULL(buff);
	buff->pos = 0;
	return 0;
}


int getBufferBytes(buffer *buff, char **bytes)
{
	if(!buff || !bytes) return -1;

	*bytes = buff->buff;
	return 0;
}
int bseekToPos(buffer *buff, int pos)
{
	if(!buff) return -1;
	if(pos > buff->pos) return -1;
	buff->pos = pos;
	return 0;
}
int getBytesCount(buffer *buff, int *count)
{
	if(!buff || !count) return -1;

	*count = buff->pos;
	return 0;
}
