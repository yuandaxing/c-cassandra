#include "queue.h"
#include "columnFamily.h"
#include "stdio.h"
#include "ssTable.h"
#include "rowMutation.h"
#include "queryPath.h"
#include "seda.h"

#define FLUSHINGTHREAD				"flushingThread"
#define NFLUSHTHREAD				1
#define COMMITLOGWRITERTHREAD		"commitLogThread"
#define NCOMMITLOGWRITERTHREAD		1
#define SSTABLEREADER				"ssTableReader"
#define NSSTABLEREADER				1

//建议该结构体添加一个锁，表明锁定mtlist,sstl
//每次从flushing中删除一个mt，增加一个sst到SSTL中
//每次总是获取ssTL所有元素的引用
typedef struct columnFamilyStore{
	memTable *curMT;
	memTableList flushingMTL;
	columnFamilyMetadata *cfmd;
	ssTableList ssTL;
	pthread_mutex_t cfsMutex;
}columnFamilyStore;

//这里析构函数设计的可能会有问题, 这样设计看上去更简洁
columnFamilyStore *getCFStore(columnFamilyMetadata *cfmd);
void freeHeapCFStore(columnFamilyStore *cfs);
int insertCFStore(columnFamilyStore *cfs, rowMutation *rm);
int  forceCFSFlush(columnFamilyStore *cfs);
column *getCFSColumn(columnFamilyStore *cfs, queryPath *qp);

void initCFSThread();
void destroyCFSThread();
