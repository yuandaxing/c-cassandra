#ifndef _SSTABLE_H_
#define _SSTABLE_H_
#include "unistd.h"
#include "stdio.h"
#include "queue.h"
#include "log.h"
#include "serialize.h"
#include "sys/types.h"
#include "pthread.h"
#include "stdlib.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "auxilFile.h"
#include "bloomFilter.h"
#include "tableMetadata.h"
#include "columnFamily.h"
#include "memTable.h"
#include "stdint.h"

#define K_1					1024
#define M_1					1024*K_1
#define DATABUFFERSZ		M_1
#define INDEXBUFFERSZ		M_1
#define FILTERBUFFERSZ		20*K_1

#define READ_TYPE			1
#define WRITE_TYPE			2
struct memTable;
int setCurSSTSN(int sn);

typedef struct ssTable{
	fileAbstract *dataFile;
	fileAbstract *indexFile;
	fileAbstract *bloomFilterFile;
	bloomFilter  *bf;
	char type;
	uint32_t refCount;
	LIST_ENTRY(ssTable) link;
}ssTable;

void decrSSTRef(ssTable *sst);
void incrSSTRef(ssTable *sst);
ssTable *openWriteSSTable(columnFamilyMetadata *cfmd, int keyCount);
//将一个key cf写入到ssTable中
long writeSSTableData(buffer *cf, ssTable *sst);
long writeSSTableIndex(buffer *index, ssTable *sst);
int writeSSTableBF(buffer *bf, ssTable *sst);
//打开一个读的ssTable, 从写磁盘后的ssTable中打开
int flushMemToSST(struct memTable *mt, ssTable *sst);
ssTable *openReadFromWrite(ssTable *sstW);
int addSSTBF(char *key, int len, ssTable *sst);
//从ssTable中读取一个cf
//columnFamily* readssTable(queryPath *qp, ssTable *ss);

//下面这两个接口主要是用于compaction
int markDelete(ssTable *sst);
//int compactSSTable(listSSTable *lss);
int deleteSSTable(ssTable *sst);
int getSSTableFromCFMD(ssTable **sst, columnFamilyMetadata *cfmd);

typedef LIST_HEAD(ssTableList, ssTable) ssTableList;
int ssTLInsert(ssTableList *sstl, ssTable *sst);
int getFirstSSTL(ssTableList *sstl, ssTable **sst);
int removeFirstSSTL(ssTableList *sstl, ssTable **sst);
void freeHeapSSTL(ssTableList *sstl);
#endif
