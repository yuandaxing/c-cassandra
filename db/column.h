/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-09-07 10:50
*
* Filename:column.h
*
* Description: 
*
=============================================================================*/
#ifndef _COLUMN_H_
#define _COLUMN_H_
#include "log.h"
#include "serialize.h"
#include "mem.h"
#include "tree.h"
#include "stdint.h"
#include "pthread.h"


#define COLUMNSEREX	(4+2) //2 表示的是序列化的长度

//可以将当前name+timestamp+del作为key，这样就可以支持不同的
//数据版本
typedef struct column{
	char *name;
	char *value;
	uint32_t timestamp;
	uint8_t del; // 1 delete, 0 not
	uint16_t size;
	uint32_t refCount;
	RB_ENTRY(column) link;
}column; 

column *getColumn();
column *cloneColumn(column *c);
void decrColumnRef(column *c);
void incrColumnCount(column *c);
void freeHeapColumn(column *x);

int serializeColumn(column *x, buffer *buff);
int deserializeColumn(buffer *buff, column **v);

int serilaizeLen(column *c);
int columnCmp(column *x, column *y);

typedef RB_HEAD(columnsHeader,column) columnsHeader; 

typedef struct columnSet{
	columnsHeader header;
	int elemCount;
}columnSet;

columnSet *getColumnSet();
int getColumnCount(columnSet *cs, int *count);
void freeHeapColumnSet(columnSet *cs);
int addColumn(columnSet *cs, column *toInsert);
int addColumnSet(columnSet *target, columnSet *src);
int serializeColumnSet(columnSet *cs, buffer *b); 
int deserializeColumnSet(columnSet *cs, buffer *b);
int  deleteColumn(columnSet *cs, column *c);
column *findCSColumn(columnSet *cs, char *name);	
#endif
