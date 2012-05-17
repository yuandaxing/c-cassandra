#ifndef _MEMTABLE_H_
#define _MEMTABLE_H_
#include "columnFamily.h"
#include "pthread.h"
#include "dict.h"
#include "ssTable.h"
#include "rowMutation.h"
#include "queue.h"
#include "queryPath.h"

#define 	INSERTEDMAX					10000
#define 	INSERTEDCOMPLETELYCF		INSERTEDMAX
#define 	INSERTEDFAILED				-1
#define 	COLUMNCOUNTTHRESHOLD		4000

struct ssTable;
typedef struct memTable{
    dict *keyCF_pair;//该字段用来存储key cf对
    int columnCount;
    int froze;
    LIST_ENTRY(memTable) link;
}memTable;

memTable *getMemTable();
void freeHeapMemTable(memTable *mt);
int insertMemTable(memTable *mt, rowMutation *rm);
column *findColumn(memTable *mt, queryPath *qp);	
int getKeyCFPairCount(memTable *mt, int *count);
int needFlush(memTable *mt);
int flushToDisk(memTable *mt, struct ssTable *sst);
///*提供一个插入，一个查询接口*/

typedef LIST_HEAD(memTableList, memTable) memTableList;

void memTLInsert(memTableList *mtl, memTable *mt);
memTable *getFirstMT(memTableList *mtl);
void removeMTFromList(memTable *mt);
void freeHeapMemTL(memTableList *mtl);

columnFamily * removeKeyFromMT(memTable *mt, char *key);
void freeKeyFromMt(memTable *mt, const char *key);

#endif
