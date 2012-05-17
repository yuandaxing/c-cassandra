#include "dataModel.h"

int insertDMCFS(columnFamilyStore *cfs);
unsigned int cfsHashing(const void *key)
{
	if(!key){
		LOG_WRITE("key here cannot be NULL\n");
		return 0;
	}

	const columnFamilyStore *cfs = (const columnFamilyStore *)key;
	return cfmdHash(cfs->cfmd);
}

int cfsCompare(void *privdata, const void *key1, 
		const void *key2)
{
	if(!key1 || !key2){
		LOG_WRITE("key1 and key cannot be NULL\n");
		return 0;
	}
	const columnFamilyStore *cfs1 = (const columnFamilyStore *)key1;
	const columnFamilyStore *cfs2 = (const columnFamilyStore *)key2;

	return cfmdCompare(privdata, cfs1->cfmd, cfs2->cfmd);
}

static void cfsKeyDestr(void *privdata, void *key)
{
	columnFamilyStore *cfs = (columnFamilyStore *)key;
	freeHeapCFStore(cfs);
}
dictType dmType = {
				cfsHashing,
				NULL,
				NULL,
				cfsCompare,
				cfsKeyDestr,
				NULL
			};

dataModel g_dataModel;

static int createAllCFSfromConfig()
{
	dictEntry *de = NULL;
	dictIterator *di = NULL;

	if(!g_dataModel.cfsHashTable)
		return -1;

	di = dictGetIterator(g_cfmDict);
	if(!di){
		return -1;
	}

	de = dictNext(di);
	while(de){
		columnFamilyMetadata *cfmd = (columnFamilyMetadata *)
							dictGetEntryKey(de);
		columnFamilyStore *cfs = NULL;

		if(!cfmd || !(cfs = getCFStore(cfmd))){
			dictReleaseIterator(di);
			return -1;
		}

		if(insertDMCFS(cfs) < 0){
			dictReleaseIterator(di);
			freeHeapCFStore(cfs);
			return -1;
		}
		de = dictNext(di);
	}
	dictReleaseIterator(di);
	return 0;
}
int __init initDM()
{
	static int initTag = 0;

	if(initTag >= 1){
		LOG_WRITE("already init\n");
		return 0;
	}

	if(!(g_dataModel.cfsHashTable = dictCreate(&dmType, NULL)))
		return -1;
	
	if(createAllCFSfromConfig() < 0){
		LOG_WRITE("cannot create cfs\n");
		return -1;
	}
	initCFSThread();
	initTag = 1;
	return 0;
}

void __exit destroyDM()
{
	destroyCFSThread();
	dictRelease(g_dataModel.cfsHashTable);
	dictRelease(g_cfmDict);
	return ;
}

columnFamilyStore *findDMCFS(char *keyspaceName, char *cfName)
{
	columnFamilyStore cfstmp = {0};
	columnFamilyMetadata tm = {keyspaceName, cfName};
	dictEntry *de = NULL;

	if(!keyspaceName || !cfName || !g_dataModel.cfsHashTable){
		return NULL;
	}

	cfstmp.cfmd = &tm;
	
	if(!(de = dictFind(g_dataModel.cfsHashTable, &cfstmp))){
		return NULL;
	}
	return (columnFamilyStore *)dictGetEntryKey(de);
}

int insertDM(rowMutation *rm)
{
	columnFamilyStore *cfs = NULL;

	if(!rm || !rm->keyspaceName || !rm->cfName){
		return -1;
	}
	if(!(cfs = findDMCFS(rm->keyspaceName, rm->cfName))){
		return -1;
	}
	return insertCFStore(cfs, rm);

}
int insertDMCFS(columnFamilyStore *cfs)
{
	if(!cfs)
		return -1;
	if(dictAdd(g_dataModel.cfsHashTable, cfs, NULL)
			== DICT_OK)
		return 0;
	return -1;
}
int forceFlushDM(char *keyspaceName, char *cfName)
{
	columnFamilyStore * cfs = NULL;

	if(!(cfs = findDMCFS(keyspaceName, cfName))){
		LOG_WRITE("cannot not find the %s %s cfstore\n",
				keyspaceName, cfName);
		return -1;
	}
	return forceCFSFlush(cfs);
}
column *getColumnDM(queryPath *path)
{
	columnFamilyStore *cfs = NULL;

	if(!path)
		return NULL;
	if(!(cfs = findDMCFS(path->keyspace, path->cfName)))
		return NULL;

	return getCFSColumn(cfs, path);
}
