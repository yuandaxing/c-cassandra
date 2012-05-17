#include "ssTable.h"
#include "buffer.h"
#include "tableMetadata.h"
#include "columnFamily.h"


static pthread_mutex_t sstMutex = PTHREAD_MUTEX_INITIALIZER;
static int curSSTSN = 0;

void freeHeapSST(ssTable *sst);

int setCurSSTSN(int sn)
{
	if(sn < 0){
		LOG_WRITE("error SSTSN\n");
		return -1;
	}
	pthread_mutex_lock(&sstMutex);
	curSSTSN = sn;
	pthread_mutex_unlock(&sstMutex);
	return 0;
}

static int incSSTSN()
{
	pthread_mutex_lock(&sstMutex);
	curSSTSN++;
	pthread_mutex_unlock(&sstMutex);
	return 0;
}

//完整的ssTable路径如下
//dataPath + “/data/"+keyspace+"/"+cfName+"-"+SSTSN+"Data.db"
//dataPath + “/data/"+keyspace+"/"+cfName+"-"+SSTSN+"Index.db"
//dataPath + “/data/"+keyspace+"/"+cfName+"-"+SSTSN+"Filter.db"
static int makeSSTablePath(const char *dataPath, char *keyspace, 
		char *cfName, char *suffix, char **path)
{
	char *p = NULL;
	char a[10] = {0};


	if(!keyspace || !cfName || !suffix || !path) 
		return -1;
	if(!(p = (char *)malloc(200)))
		return -1;

	strcpy(p, dataPath);
	strcat(p, "/data/");
	strcat(p, keyspace);
	strcat(p, "/");
	strcat(p, cfName);
	pthread_mutex_lock(&sstMutex);
	sprintf(a, "-%d-", curSSTSN);
	pthread_mutex_unlock(&sstMutex);
	strcat(p, a);
	strcat(p, suffix);

	*path = p;
	return 0;
}

void decrSSTRef(ssTable *sst)
{
	if(!sst)
		return ;
	pthread_mutex_lock(&sstMutex);
	sst->refCount--;
	if(!sst->refCount)
		freeHeapSST(sst);
	pthread_mutex_unlock(&sstMutex);
}
void incrSSTRef(ssTable *sst)
{
	if(!sst)
		return ;
	pthread_mutex_lock(&sstMutex);
	sst->refCount++;
	pthread_mutex_unlock(&sstMutex);
}

void freeHeapSST(ssTable *sst)
{
	if(!sst) return;

	if(sst->dataFile) freeHeapFile(sst->dataFile);
	if(sst->indexFile) freeHeapFile(sst->indexFile);
	if(sst->bloomFilterFile) freeHeapFile(sst->bloomFilterFile);
	if(sst->bf) freeHeapBF(sst->bf);

	free(sst);
}
ssTable *openWriteSSTable(columnFamilyMetadata *cfmd, int keyCount)
{
	char *keyspace = NULL;
	char *cfName = NULL;
	const char *dataPath = NULL;
	char *dpath = NULL, *Inpath = NULL,*filterpath = NULL;
	ssTable *sstemp = NULL;

	if(!cfmd || keyCount <=0 ){
		LOG_WRITE("arg error \n");
		return NULL;
	}

	keyspace = cfmd->keyspaceName;
	cfName = cfmd->cfName;

	if(cfmd == NULL) 
		return NULL;

	if(getDataPath(&dataPath) < 0){
		return NULL;
	}
	
	if(!(sstemp = (ssTable *)calloc(sizeof(ssTable), 1)))
		return NULL;

	if(makeSSTablePath(dataPath, keyspace, cfName, "Data.db", 
														&dpath)  < 0||
			makeSSTablePath(dataPath, keyspace, cfName, "Index.db", 
														&Inpath)  < 0|| 
			makeSSTablePath(dataPath, keyspace, cfName, "Filter.db", 
													&filterpath) < 0){
		decrSSTRef(sstemp);
		if(dpath) free(dpath);
		if(Inpath) free(Inpath);
		if(filterpath) free(filterpath);
		return NULL;
	}

	//printf("data file is %s\n", dpath);
	if(openWriteFile(dpath, DATABUFFERSZ, &sstemp->dataFile) < 0 ||
		openWriteFile(Inpath, INDEXBUFFERSZ, &sstemp->indexFile) < 0 ||
			openWriteFile(filterpath, FILTERBUFFERSZ, 
						&sstemp->bloomFilterFile) < 0 ||
			getBloomFilter(KEYHASHCOUNT, keyCount, &sstemp->bf) < 0){
			decrSSTRef(sstemp);
			free(dpath);
			free(Inpath);
			free(filterpath);
			return NULL;
	}

	incSSTSN();
	free(dpath);
	free(Inpath);
	free(filterpath);

	sstemp->type = WRITE_TYPE;
	sstemp->refCount++;
	sstemp->refCount = 1;

	return sstemp;
}
//如果写入失败，就返回-1，写入成功，返回写入的位置
long writeSSTableData(buffer *cf, ssTable *sst)
{
	long curPos = 0;

	if(!cf || !sst) 
		return -1;
	if(!sst->dataFile || !sst->indexFile)
		return -1;
	if((curPos = getCurFilePos(sst->dataFile))
		   	< 0){
		return -1;
	}
	if(writeAppendFile(sst->dataFile, cf) < 0)
		return -1;

	return curPos;
}
//如果写入失败，就返回-1，写入成功，返回写入的位置
long writeSSTableIndex(buffer *index, ssTable *sst)
{
	long curPos = 0;
	if(!index || !sst) 
		return -1;
	if(!sst->indexFile)
		return -1;
	if((curPos = getCurFilePos(sst->indexFile))
		   	< 0)
		return -1;
	if(writeAppendFile(sst->indexFile, index) < 0)
		return -1;
	return curPos;
}
int writeSSTableBF(buffer *bf, ssTable *sst)
{
	if(!bf || !sst)
		return -1;
	if(!sst->bloomFilterFile)
		return -1;
	if(writeAppendFile(sst->bloomFilterFile, bf)
				< 0)
		return -1;
	return 0;
}

int addSSTBF(char *key, int len, ssTable *sst)
{
	if(!sst || !sst->bf) return -1;
	return addToBF(sst->bf, key, len);
}
//调用这个函数处理的时候，ssWriter会被关闭，然后ssReader用来查询
ssTable *openReadFromWrite(ssTable *sstW)
{
	buffer *buff = NULL;
	ssTable *tmp = NULL;

	if(!sstW || sstW->type != WRITE_TYPE || 
			 !sstW->dataFile ||
			!sstW->indexFile || !sstW->bloomFilterFile || 
			!sstW->bf)
		return NULL;

	if(!(buff = getBuffer())){
		return NULL;
	}

	if(serializeBF(buff, sstW->bf) < 0 || 
			writeSSTableBF(buff, sstW) < 0){
		freeHeapBuffer(buff);
		return  NULL;
	}
	freeHeapBuffer(buff);

	if(!(tmp = (ssTable *)calloc(sizeof(ssTable), 1)) || 
			openReaderFileFromExist(sstW->dataFile,DATABUFFERSZ, 
				&tmp->dataFile) < 0 ||
	 		openReaderFileFromExist(sstW->indexFile, INDEXBUFFERSZ, 
			   &tmp->indexFile) < 0 || !(tmp->bf = 
				   dupBloomFilter(sstW->bf))){
		if(tmp)
			decrSSTRef(tmp);
		return NULL;
	}

	tmp->type = READ_TYPE;
	tmp->refCount = 1;
	return tmp;
}
int ssTLInsert(ssTableList *sstl, ssTable *sst)
{	
	if(!sstl || !sst)
		return -1;

	LIST_INSERT_HEAD(sstl, sst, link);
	return  0;
}
int getFirstSSTL(ssTableList *sstl, ssTable **sst)
{
	if(!sstl || !sst)
		return -1;
	if(LIST_EMPTY(sstl))
		return -1;

	*sst = LIST_FIRST(sstl);
	return 0;
}
int removeFirstSSTL(ssTableList *sstl, ssTable **sst)
{
	ssTable *cursst = NULL;
	if(!sstl)
		return -1;
	if(getFirstSSTL(sstl, &cursst) < 0)
		return -1;
	if(sst){
		*sst = cursst;
	}else{
		decrSSTRef(cursst);
	}
	return 0;
}

void freeHeapSSTL(ssTableList *sstl)
{
	while(removeFirstSSTL(sstl, NULL) == 0) 
		; 
}
