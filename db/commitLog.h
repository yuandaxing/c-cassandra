#ifndef _COMMITLOG_H_
#define _COMMITLOG_H_
#include "stdio.h"
#include "auxilFile.h"
#include "tableMetadata.h"
#include "serialize.h"
#include "queue.h"


#define READ_COMMITLOG	0
#define WRITE_COMMITLOG	1
#define COMMITLOGBUFFERSZ	1024*1024*10
#define COMMITLOGMAXSZ		1024*1024*200

struct clHeader;
typedef struct commitLog{
	fileAbstract *fa;
	struct clHeader *clh;
	LIST_ENTRY(commitLog) link;
}commitLog;

void freeHeapCommitLog(commitLog *cl);
int writeCommitLog(buffer *rm,const columnFamilyMetadata *cfmd);
int recoverFromCommitLog();
int synCommitLog(columnFamilyMetadata *cfmd);
#endif
