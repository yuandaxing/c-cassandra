#include "stdio.h"
#include "serialize.h"
#include "log.h"
#include "string.h"


int writeInt(int data, buffer *buff)
{
	int i;

	if(!buff){
		LOG_WRITE("pointer buffer should not be NULL\n");
		return -1;
	}
	if(reserve(buff, INTLEN) < 0){
		LOG_WRITE("OUT of Mem\n");
		return -1;
	}

	for(i = 0; i < 4; i++){
		buff->buff[buff->pos++] = GET_BYTE(data, i);
	}

	return 0;
}
int readInt(buffer *buff, int *v)
{
	POINTER_NOT_NULL(buff);
	NOT_LESS_THAN(buff->size, buff->pos + 4);

	int ret = 0;

	ret += PUT_BYTE(buff->buff[buff->pos++], 0);
	ret += PUT_BYTE(buff->buff[buff->pos++], 1);
	ret += PUT_BYTE(buff->buff[buff->pos++], 2);
	ret += PUT_BYTE(buff->buff[buff->pos++], 3);
	if(v) *v = ret;
	return 0;
}

int writeLong(long data, buffer *buff)
{

	POINTER_NOT_NULL(buff);
	if(reserve(buff, INTLEN) < 0){
		LOG_WRITE("OUT of Mem\n");
		return -1;
	}

	int i;
	for(i = 0; i < 4; i++){
		buff->buff[buff->pos++] = GET_BYTE(data, i);
	}

	return 4;
}
int readLong(buffer *buff, long *v)
{
	POINTER_NOT_NULL(buff);
	NOT_LESS_THAN(buff->size, buff->pos + 4);

	long ret = 0;

	ret += PUT_BYTE(buff->buff[buff->pos++], 0);
	ret += PUT_BYTE(buff->buff[buff->pos++], 1);
	ret += PUT_BYTE(buff->buff[buff->pos++], 2);
	ret += PUT_BYTE(buff->buff[buff->pos++], 3);
	if(v) *v = ret;
	return 0;
}
int writeUnsignedInt(unsigned int data, buffer *buff)
{
	POINTER_NOT_NULL(buff);
	if(reserve(buff, INTLEN) < 0){
		LOG_WRITE("OUT of Mem\n");
		return -1;
	}

	int i;
	for(i = 0; i < 4; i++){
		buff->buff[buff->pos++] = GET_BYTE(data, i);
	}

	return 4;
}
int readUnsignedInt(buffer *buff, unsigned int *v)
{
	POINTER_NOT_NULL(buff);
	NOT_LESS_THAN(buff->size, buff->pos + 4);

	unsigned int ret = 0;;

	ret += PUT_BYTE(buff->buff[buff->pos++], 0);
	ret += PUT_BYTE(buff->buff[buff->pos++], 1);
	ret += PUT_BYTE(buff->buff[buff->pos++], 2);
	ret += PUT_BYTE(buff->buff[buff->pos++], 3);
	if(v) *v = ret;
	return 0;
}

/* we do not write \0 character */
int writeBytes(char *str, int len, buffer *buff)
{
	if(!buff){
		LOG_WRITE("buffer should not be NULL\n");
		return -1;
	}

	if(reserve(buff, len + INTLEN)){
		LOG_WRITE("out of mem\n");
		return -1;
	}
	writeInt(len, buff);
	memcpy(buff->buff + buff->pos , str, len);
	buff->pos += len;
	return 0;
}
/* 
   to read bytes, first we should read len, then alloc an array to 
   hold the bytes, 如果是字符串，序列化应该加上 \0
 */
int readBytes(buffer *buff, char **v, int *lenv)
{
	POINTER_NOT_NULL(buff);
	NOT_LESS_THAN(buff->size, buff->pos + 4);
	int len;
  	readInt(buff, &len);
	
	char *bytes = (char *)malloc(len);
	if(!bytes) return -1;

	memcpy(bytes, buff->buff + buff->pos, len);
	buff->pos += len;

	if(v) 
		*v = bytes;
	else
		free(bytes);	

	if(lenv)
		*lenv = len;
	return 0;
}

int writeByte(char byte, buffer *buff)
{
	POINTER_NOT_NULL(buff);
	if(reserve(buff, 1)){
		LOG_WRITE("out of mem\n");
		return -1;
	}
	buff->buff[buff->pos++] = byte;
	return 0;
}
int readByte(buffer *buff, char *v)
{
	POINTER_NOT_NULL(buff);
	NOT_LESS_THAN(buff->size, buff->pos + 1);
	char byte = buff->buff[buff->pos++];
	if(v) *v = byte;
	return 0;
}
