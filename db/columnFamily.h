/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-09-08 16:17
*
* Filename:columnFamily.h
*
* Description: 
*
=============================================================================*/
#ifndef _COLUMNFAMILY_H_
#define _COLUMNFAMILY_H_
#include "column.h"
#include "superColumn.h"
#include "tableMetadata.h"
#include "stdint.h"

typedef struct columnFamily{
	void *columnorSuper;
	uint32_t markedForDelete;
	uint32_t localDelete;
	columnFamilyMetadata *cfm;
}columnFamily;

//cf构造析构函数
int initCF(columnFamily *cf, columnFamilyMetadata *cfmd);
columnFamily *getCF(columnFamilyMetadata *cfm);
void clearColumnFamily(columnFamily *cf);
void freeHeapColumnFamily(columnFamily *cf);
//动态申请cf，必须使用该接口
int getCFType(columnFamily *cf, enum cfType *v);
int getCFID(columnFamily *cf, int *ID);
int getCFColumnCount(columnFamily *cf, int *count);
char *getCFName(columnFamily *cf);
char *getCFKeyspace(columnFamily *cf);

//序列化接口
int serializeCF(columnFamily *cf, buffer *buff);
int deserializeCF(buffer *buff, columnFamily *cf);

//插入成功返回插入column的数量 , 失败返回-1
int insertCF(columnFamily *cf, columnFamily *inserted);
int addCFColumn(columnFamily *cf, column *toInsert);
column *findCFColumn(columnFamily *cf, char *columName);
#endif
