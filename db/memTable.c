#include "memTable.h"
#include "string.h"

typedef struct sortedDE{
	dictEntry **ep;
	int size;
}sortedDE;

void freeHeapSortedDE(sortedDE *de);
sortedDE *getSortedEntry(memTable *mt);

void freeHeapSortedDE(sortedDE *de)
{
	if(!de)
		return ;
	if(de->ep)
		free(de->ep);
	free(de);
}

/*this is for sorted the key/cf pairs*/
static int keyCompare(const void *e1, const void *e2)
{
	const dictEntry **x = (const dictEntry**)e1;
	const dictEntry **y = (const dictEntry**)e2;
	return strcmp((char *)(*x)->key, (char *)(*y)->key);
}

extern int getKeyCFPairCount(memTable *mt, int *count);
sortedDE *getSortedEntry(memTable *mt)
{
	int keyCFCount = 0, i;
	sortedDE *tmp = NULL;
	dictIterator *di = NULL;

	if(!mt){
		LOG_WRITE("mem table could not be null\n");
		return NULL;
	}

	if(getKeyCFPairCount(mt, &keyCFCount) < 0){
		return  NULL;
	}

	if(!(tmp = (sortedDE*)calloc(sizeof(sortedDE), 1))){
		LOG_WRITE("cannot alloc struct sorted DE\n");
		return NULL;
	}

	if(!(tmp->ep = (dictEntry **)malloc(sizeof(dictEntry *) 
						* keyCFCount))){
		freeHeapSortedDE(tmp);	
		return  NULL;
	}
	if(!(di = dictGetIterator(mt->keyCF_pair))){
		freeHeapSortedDE(tmp);	
		return NULL;
	}
	tmp->size = keyCFCount;
	for(i = 0; i < keyCFCount; i++){
		tmp->ep[i] = dictNext(di);
	}

	dictReleaseIterator(di);
	qsort(tmp->ep, tmp->size, sizeof(dictEntry *), keyCompare);

	return tmp;
}

/*下面的主要是为了实现hash表key/cf对*/
/*do not need check pointer */
static unsigned int hashFunc(const void *key)
{
	const unsigned char *str = (const unsigned char *)key;
	return dictGenHashFunction(str, strlen((const char *)str));
}

static int keyCF_pairCompare(void *privdata, const void *key1, const void *key2)
{
	const char *k1 = (const char *)key1;
   	const char *k2 = (const char *)key2;
	return strcmp(k1, k2) == 0;
}
static void keyDestructor(void *privdata, void *key)
{
	free(key);
}
static void valDestructor(void *privdata, void *ob)
{
	columnFamily *cf = (columnFamily*)ob;
	freeHeapColumnFamily(cf);
}

static dictType memTableType = {
									hashFunc, 
									NULL, 
									NULL,
									keyCF_pairCompare, 
									keyDestructor, 
									valDestructor
								};

memTable *getMemTable()
{
	memTable *mt = NULL;
	dict *key_cf = NULL;


	if(!(mt = (memTable*)calloc(sizeof(memTable), 1))){
		return NULL;
	}

	if(!(key_cf = dictCreate(&memTableType, NULL))){
		free(mt);
		LOG_WRITE("cannot create dict\n");
		return NULL;
	}

	*mt = (memTable){key_cf};
	return mt;
}

void freeHeapMemTable(memTable *mt)
{
	if(!mt) 
		return ;
	if(mt->keyCF_pair) dictRelease(mt->keyCF_pair);
	free(mt);
}

int putKeyCFPair(memTable *mt, char *key, columnFamily *cf)
{
	if(!mt || !mt->keyCF_pair || !key || !cf) 
		return -1;
	
	if(dictAdd(mt->keyCF_pair, (void *)key, 
				(void *)cf) == DICT_ERR){
		return -1;

	}

	return 0;
}

columnFamily * removeKeyFromMT(memTable *mt, char *key)
{
	dictEntry *de = NULL;
	columnFamily *cf = NULL;

	if(!mt || !key || !mt->keyCF_pair) 
		return NULL;
	
	if(!(de = dictFind(mt->keyCF_pair, (const void *)key)))	
		return NULL;
	if(dictDeleteNoFree(mt->keyCF_pair, (const void *)key) == 
		DICT_ERR){
		return NULL;
	}	

	cf = (columnFamily *)dictGetEntryVal(de);
	dictFreeEntryKey(mt->keyCF_pair, de);
	free(de);

	return cf;
}
void freeKeyFromMt(memTable *mt, const char *key)
{
	if(!mt || !mt->keyCF_pair || !key) 
		return ;
	dictDelete(mt->keyCF_pair, (void *)key); 
}

int flushToDisk(memTable *mt, struct ssTable *sst)
{
	int i;
	long pos;
	sortedDE *sde = NULL;
	buffer *data = NULL, *index = NULL;

	if(!mt || !sst) 
		return -1;
	if(!sst->dataFile || !sst->indexFile || 
			!sst->bloomFilterFile)
		return -1;

	if(!(sde = getSortedEntry(mt)))
		return -1;

	if(!(data = getBuffer()) ||
			!(index = getBuffer())){
		freeHeapSortedDE(sde);
		freeHeapBuffer(data);
		freeHeapBuffer(index);
	}

	for(i = 0; i < sde->size; i++){
		char *key = (char *)sde->ep[i]->key;
		columnFamily *cf = (columnFamily*)sde->ep[i]->val;

		if(writeBytes(key, strlen(key) + 1, data) < 0 || 
				serializeCF(cf, data) < 0 || 
			(pos = writeSSTableData(data, sst)) < 0){
			freeHeapSortedDE(sde);
			freeHeapBuffer(data);
			freeHeapBuffer(index);	
			return -1;
		}

		if(writeBytes(key, strlen(key) + 1, index) < 0 ||
				writeLong(pos, index) < 0 ||
				addSSTBF(key, strlen(key), sst) < 0){
			freeHeapSortedDE(sde);
			freeHeapBuffer(data);
			freeHeapBuffer(index);	
			return -1;
		}
		reset(data);
	}
	freeHeapSortedDE(sde);
	freeHeapBuffer(data);

	if((pos = writeSSTableIndex(index, sst)) < 0){
		freeHeapBuffer(index);
		return -1;
	}

	freeHeapBuffer(index);
	return 0;
}
//返回值有三种，一种是将cf，key完全插入到memtable中,即为INSERTEDCOMPLETELYCF
//一种是插入失败，返回值为INSERTEDFAILED
//最后是插入了一些column或者一个column也没有插入进去，返回插入的column数量
int insertMemTable(memTable *mt, rowMutation *rm)
{
	dictEntry *de = NULL;
	columnFamily *cf = NULL;
	int columnAdd = 0;

	if(!mt || !mt->keyCF_pair || !rm ||
			!rm->key || !rm->cf)
		return -1;	

	//可以直接将key，cf写入到mt
	getCFColumnCount(rm->cf, &columnAdd);
	if(putKeyCFPair(mt, rm->key, rm->cf) == DICT_OK){
		rm->key = NULL;
		rm->cf = NULL;
		mt->columnCount += columnAdd;
		return columnAdd;
	}
	//这里应该一定能找到
	if(!(de = dictFind(mt->keyCF_pair, rm->key))){
		return -1;
	}
	cf = (columnFamily *)dictGetEntryVal(de);
	columnAdd = insertCF(cf, rm->cf);
	mt->columnCount += columnAdd;
	return columnAdd;
}

int getMTColumnCount(memTable *mt)
{
	if(!mt || !mt->keyCF_pair) 
		return -1;
	return mt->columnCount;

	return 0;
}
int getKeyCFPairCount(memTable *mt, int *count)
{
	if(!mt || !count)
		return -1;
	if(!mt->keyCF_pair) 
		return -1;
	*count = dictSize(mt->keyCF_pair);
	return 0;
}

/*-1 error, 0 no, 1 yes */
int needFlush(memTable *mt)
{
	int count;

	if(!mt)
		return -1;
	if((count = getMTColumnCount(mt)) < 0)
		return -1;
	if(count > COLUMNCOUNTTHRESHOLD)
		return 1;
	return  0;
}

columnFamily *findMTCF(memTable *mt, queryPath *qp)
{
	dictEntry *de = NULL;

	if(!mt || !qp || !mt->keyCF_pair)
		return NULL;
	
	de = dictFind(mt->keyCF_pair, qp->key);
	if(de)
		return (columnFamily *)dictGetEntryVal(de);
	return NULL;
}
column *findColumn(memTable *mt, queryPath *qp)
{
	columnFamily *cf = NULL;

	if(!mt || !qp)
		return NULL;
	if(!(cf = findMTCF(mt, qp)))
		return NULL;

	return findCFColumn(cf, qp->columnName);	
}

void memTLInsert(memTableList *mtl, memTable *mt)
{
	if(!mtl || !mt){
		LOG_WRITE("arg error\n");
		return ;
	}
	LIST_INSERT_HEAD(mtl, mt, link);
	return ;
}
memTable *getFirstMT(memTableList *mtl)
{
	if(!mtl){
		return NULL;
	}
	if(LIST_EMPTY(mtl)){
		return NULL;
	}
	return LIST_FIRST(mtl);
}

void removeMTFromList(memTable *mt)
{

	if(!mt){
		return ;
	}
	LIST_REMOVE(mt, link);
}
void freeHeapMemTL(memTableList *mtl)
{
	memTable *tmp = NULL;
	if(!mtl){
		return ;
	}
	tmp = getFirstMT(mtl);
	while(tmp != NULL){
		removeMTFromList(tmp);
		freeHeapMemTable(tmp);
		tmp = getFirstMT(mtl);
	}
}
