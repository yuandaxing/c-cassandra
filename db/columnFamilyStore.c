#include "stdio.h"
#include <pthread.h>
#include <time.h>
#include "columnFamilyStore.h"
#include "commitLog.h"
#include "errno.h"

typedef struct flushTask{
	int clHeaderUpdate;
	int refCount;
	pthread_mutex_t mutex;
	pthread_cond_t signal;
	columnFamilyStore *cfs;
	memTable *mt;
}flushTask;

void decrFlushTask(flushTask *fl);
void freeHeapFL(flushTask *fl);
void incrFlushTask(flushTask *fl);

flushTask *getFlushTask(columnFamilyStore *cfs, memTable *mt);
void flushingHandler(void *arg);
void synFlushingTask(void *arg);

flushTask *getFlushTask(columnFamilyStore *cfs, memTable *mt)
{
	flushTask *ft = NULL;

	if(!cfs || !mt){
		printf("argment error\n");
		return NULL;
	}

	if(!(ft = calloc(sizeof(flushTask), 1))){
		LOG_WRITE("oom\n");
		return NULL;
	}
	ft->refCount = 1;
	ft->mt = mt;
	ft->cfs = cfs;
	pthread_mutex_init(&ft->mutex, NULL);
	pthread_cond_init(&ft->signal, NULL);
	return ft;
}

void incrFlushTask(flushTask *ft)
{
	if(!ft){
		return ;
	}
	ft->refCount++;
}
void decrFlushTask(flushTask *ft)
{
	if(!ft)
		return ;
	ft->refCount--;
	if(ft->refCount == 0)
		freeHeapFL(ft);
}
void freeHeapFL(flushTask *ft)
{
	if(!ft || ft->refCount != 0){
		return ;
	}
	pthread_mutex_destroy(&ft->mutex);
	pthread_cond_destroy(&ft->signal);
	free(ft);
}

void flushingHandler(void *arg)
{
	flushTask *ft = NULL;
	ssTable *ssWriter = NULL, *ssReader = NULL;
	int keyCount;
	time_t tt;
	struct timespec timeOut;
	int retCode = 0;

	if(!(ft = (flushTask *)arg)){
		LOG_WRITE("args cannot be NULL in flushing\n");
		return ;
	}

	if(!ft->mt ||getKeyCFPairCount(ft->mt, &keyCount) < 0 ||
			!ft->cfs || !ft->cfs->cfmd ||
			!(ssWriter = openWriteSSTable(ft->cfs->cfmd, 
				keyCount))){
		//打开ssTable失败
		decrFlushTask(ft);
		return ;
	}
	/*
	   这里是一个耗时的操作，应该采用seda的方法将方法包装
	   成一个任务，提交给线程池处理, 在flushtodisk 之前应该考虑修改
	   commitLog的context
	 */
	if(flushToDisk(ft->mt, ssWriter/*writeSSTable*/) < 0){  
		decrFlushTask(ft);
		decrSSTRef(ssWriter);
		return ;
	}

	if(!(ssReader = openReadFromWrite(ssWriter))){
		//我们丢失了写入的数据，只有下次启动的时候读入
		decrFlushTask(ft);
		decrSSTRef(ssWriter);
		return ;
	}
	decrSSTRef(ssWriter);
	//这里没有插入这里要求同步

	tt = getCurTime() + 60;
	timeOut.tv_sec = tt;
	timeOut.tv_nsec = 0;
	pthread_mutex_lock(&ft->mutex);
	while(ft->clHeaderUpdate == 0 && 
			retCode != ETIMEDOUT)
		retCode = pthread_cond_timedwait(&ft->signal, 
				&ft->mutex,	&timeOut);
	pthread_mutex_unlock(&ft->mutex);

	if(retCode != ETIMEDOUT || 
			ft->clHeaderUpdate == 1){
		pthread_mutex_lock(&ft->cfs->cfsMutex);
		if(ssTLInsert(&ft->cfs->ssTL, ssReader) < 0){
			decrFlushTask(ft);
			decrSSTRef(ssReader);
			pthread_mutex_unlock(&ft->cfs->cfsMutex);
			return ;
		}
		LIST_REMOVE(ft->mt, link);
		pthread_mutex_unlock(&ft->cfs->cfsMutex);
		freeHeapMemTable(ft->mt);
	}else{
		//删除ssReader, 删除文件没有实现
		decrSSTRef(ssReader);
	}
	decrFlushTask(ft);
}

void synFlushingTask(void *arg)
{
	flushTask *ft = (flushTask *)arg;
	columnFamilyMetadata *cfmd = NULL;

	if(!ft){
		LOG_WRITE("arg error\n");
		return ;
	}
	if(!ft->cfs || !(cfmd = ft->cfs->cfmd)){
		LOG_WRITE("arg error\n");
		decrFlushTask(ft);
		return ;
	}
	
	if(synCommitLog(cfmd) < 0){
		LOG_WRITE("cannot syn commitLog\n");
		decrFlushTask(ft);
		return ;
	}

	//线程同步
	pthread_mutex_lock(&ft->mutex);
	ft->clHeaderUpdate = 1;
	pthread_mutex_unlock(&ft->mutex);
	pthread_cond_signal(&ft->signal);
	decrFlushTask(ft);
}



typedef struct commitLogWriter{
	buffer *buff;
	columnFamilyMetadata *cfmd;
}commitLogWriter;

commitLogWriter *getCLWriter(buffer *buff, 
		columnFamilyMetadata *cfmd);
void clWriterHandler(void *arg);
void freeHeapCLWriter(commitLogWriter *clw);

void freeHeapCLWriter(commitLogWriter *clw)
{
	if(!clw)
		return ;
	freeHeapBuffer(clw->buff);
	free(clw);
}
commitLogWriter *getCLWriter(buffer *buff, 
		columnFamilyMetadata *cfmd)
{
	commitLogWriter *clw = NULL;
	if(!(clw = (commitLogWriter *)malloc(sizeof(
						commitLogWriter)))){
		return NULL;
	}
	clw->buff = buff;
	clw->cfmd = cfmd;
	return clw;
}

void clWriterHandler(void *arg)
{
	commitLogWriter *clw = (commitLogWriter *)arg;
	if(writeCommitLog(clw->buff, clw->cfmd) < 0){
		LOG_WRITE("write commitLogw error\n");
	}
	freeHeapCLWriter(clw);
}

//ssTable在初始化的时候加载，可以设计为统一加载，或者在加载cfStore的时候加载
columnFamilyStore *getCFStore(columnFamilyMetadata *cfmd)
{
	memTable *mt = NULL;
	columnFamilyStore *cfs = NULL;

	if(!cfmd)
		return  NULL;
	if(!(mt = getMemTable()))
		return NULL;
	if(!(cfs = (columnFamilyStore *)calloc(sizeof(columnFamilyStore)
					, 1))){
		freeHeapMemTable(mt);
		return NULL;
	}

	cfs->curMT = mt;
	cfs->cfmd = cfmd;
	pthread_mutex_init(&cfs->cfsMutex, NULL);
	return cfs;
}

void freeHeapCFStore(columnFamilyStore *cfs)
{
	if(!cfs) return ;
	freeHeapSSTL(&cfs->ssTL);
	freeHeapMemTL(&cfs->flushingMTL);
	freeHeapMemTable(cfs->curMT);
	pthread_mutex_destroy(&cfs->cfsMutex);
	free(cfs);
}

//写入的步骤
//先写commitLog，然后写入memTable，最后如果MemTable要求刷新，
//则将memTable中的数据写入ssTable
int insertCFStore(columnFamilyStore *cfs, rowMutation *rm)
{
	buffer *buff = NULL;
	int ret = 0;
	commitLogWriter *clw = NULL;

	if(!cfs || !rm ){
		return -1;
	}
	if(!(buff = getBuffer())){
		return -1;
	}

	//这里的写commitLog失败不好处理，我们祈祷写成功吧
	if(serializeRM(buff, rm) < 0 ||
			(clw = getCLWriter(buff, cfs->cfmd)) == NULL || 
			dispatchStage(COMMITLOGWRITERTHREAD, 
				clWriterHandler, clw) < 0){
		if(clw) 
			freeHeapCLWriter(clw);
		else
			freeHeapBuffer(buff);
		return -1;
	}

	pthread_mutex_lock(&cfs->cfsMutex);
	ret = insertMemTable(cfs->curMT, rm);

	if(needFlush(cfs->curMT) == 1){
		flushTask *ft = NULL;

		if(!(ft = getFlushTask(cfs,cfs->curMT))){
			pthread_mutex_unlock(&cfs->cfsMutex);
			LOG_WRITE("oom\n");
			return -1;
		}

		memTLInsert(&cfs->flushingMTL, cfs->curMT);
		if(!(cfs->curMT = getMemTable(cfs->cfmd))){
			pthread_mutex_unlock(&cfs->cfsMutex);
			LOG_WRITE("error in cfsStore\n");
			return -1;
		}
		//这里应该先dispatch一个commitLog

		incrFlushTask(ft);
		if(dispatchStage(FLUSHINGTHREAD, flushingHandler, ft) < 0
				|| dispatchStage(COMMITLOGWRITERTHREAD,
					synFlushingTask, ft) < 0){
			pthread_mutex_unlock(&cfs->cfsMutex);
			LOG_WRITE("cannot dispatch thread\n");
			exit(0);
		}
	}
	pthread_mutex_unlock(&cfs->cfsMutex);

	return ret;
}

//在将memTableflush到磁盘上，我们将这个过程交给你个线程处理
//这一过程并不是reference传递的过程，我们认为这是一个归属的问题,
//我们认为将当前memTable的归属传递给线程处理
int forceCFSFlush(columnFamilyStore *cfs)
{
	flushTask *ft = NULL;

	if(!cfs)
		return -1;

	pthread_mutex_lock(&cfs->cfsMutex);
	if(!(ft = getFlushTask(cfs,cfs->curMT))){
		pthread_mutex_unlock(&cfs->cfsMutex);
		LOG_WRITE("oom\n");
		return -1;
	}

	memTLInsert(&cfs->flushingMTL, cfs->curMT);
	if(!(cfs->curMT = getMemTable(cfs->cfmd))){
		pthread_mutex_unlock(&cfs->cfsMutex);
		LOG_WRITE("error in cfsStore\n");
		return -1;
	}

	incrFlushTask(ft);
	if(dispatchStage(FLUSHINGTHREAD, flushingHandler, ft) < 0
			|| dispatchStage(COMMITLOGWRITERTHREAD,
				synFlushingTask, ft) < 0){
		pthread_mutex_unlock(&cfs->cfsMutex);
		LOG_WRITE("cannot dispatch thread\n");
		exit(0);
	}
	pthread_mutex_unlock(&cfs->cfsMutex);
	//这里应该考虑加锁
	return 0;
}
//这里只设计一个初步的
column *getCFSColumn(columnFamilyStore *cfs, queryPath *qp)
{
	memTable *mt = NULL;
	column *curCol = NULL;


	if(!cfs || !qp)
		return NULL;

	mt = cfs->curMT;
	if(mt)
		curCol = findColumn(mt, qp);	

	return curCol;
}
static int flag = 0;
void  initCFSThread()
{
	if(flag)
		return ;
	
	if(register_stage(FLUSHINGTHREAD, NFLUSHTHREAD) < 0 ||
			register_stage(COMMITLOGWRITERTHREAD,
				NCOMMITLOGWRITERTHREAD) < 0 ||
			register_stage(SSTABLEREADER, NSSTABLEREADER) < 0){
		LOG_WRITE("cannot init cfs thread\n");
		exit(0);
	}
	flag = 1;
}

void destroyCFSThread()
{
	if(deallocStage(COMMITLOGWRITERTHREAD) < 0 ||
			deallocStage(FLUSHINGTHREAD) < 0 ||
			deallocStage(SSTABLEREADER) < 0){
		LOG_WRITE("cannot stop the thread\n");
	}
}
