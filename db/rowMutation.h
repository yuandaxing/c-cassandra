/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-09-08 16:18
*
* Filename:rowMutation.h
*
* Description: 
*
=============================================================================*/
#ifndef _ROWMUTATION_H_
#define _ROWMUTATION_H_
#include "stdio.h"
#include "serialize.h"
#include "columnFamily.h"
#include "tableMetadata.h"

typedef RB_HEAD(columnFamilySet, columnFamily) columnFamilySet;

typedef struct rowMutation{
	char *keyspaceName;
	char *key;
	char *cfName;
	columnFamily *cf;
}rowMutation;

//初始化以及析构rm
rowMutation *getRM();
void freeHeapRM(rowMutation *rm);
//序列化，反序列化rm
int serializeRM(buffer *b, rowMutation *rm);
int deserializeRM(buffer *b, rowMutation *rm);
int getRMCFID(rowMutation *rm, int *id);
//插入
int insertRM(rowMutation *rm);

#endif
