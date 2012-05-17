/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-08-29 15:17
*
* Filename:serialize.h
*
* Description: 这里主要提供了序列化和反序列化的一些基本函数
*			writeXXX 将数据写到buff中，len是buff的长度，返回写入的长度
*			readXXX	 读取buff中的数据，返回读取的数据，对于读取byte，返回byte的长度，数据在byte参数中
*
=============================================================================*/
#include "string.h"
#include "buffer.h"

#define		INTLEN		4
#define		LONGLEN		4
#define		UNSIGNLEN	4
#define		BYTELEN		1
#define		BYTESHEADLEN	4
#define		COLUMNHEADLEN	4



#define GET_BYTE(DATA, INDEX)	(DATA >> (INDEX * 8) & 0xFF)
#define PUT_BYTE(BYTE, INDEX)	((unsigned char)BYTE << (INDEX * 8))



int writeInt(int data, buffer *buff);
int readInt(buffer *buff, int *v);
int writeBytes(char *str, int len, buffer *buff);
int readBytes(buffer *buff, char **v, int *len);
int writeLong(long data, buffer *buff);
int readLong(buffer *buff, long *v);
int writeUnsignedInt(unsigned int data, buffer *buff);
int readUnsignedInt(buffer *buff, unsigned int *v);
int writeByte(char byte, buffer *buff);
int readByte(buffer *buff, char *v);

