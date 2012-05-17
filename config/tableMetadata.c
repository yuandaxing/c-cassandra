#include "tableMetadata.h"
#include "dict.h"
#include "string.h"
#include "stdlib.h"
#include "log.h"



unsigned int cfmdHash(const void *key)
{
	const columnFamilyMetadata *cfm = (const columnFamilyMetadata *)key;
	return dictGenHashFunction((const unsigned char *)cfm->keyspaceName, 
			strlen(cfm->keyspaceName)) ^
		   dictGenHashFunction((const unsigned char *)cfm->cfName, 
			strlen(cfm->cfName));
}

int cfmdCompare(void *privdata, const void *key1, const void *key2)
{
	const columnFamilyMetadata *cf1 = NULL, *cf2 = NULL;
	cf1 = (const columnFamilyMetadata *)key1;
	cf2 = (const columnFamilyMetadata *)key2;
	return strcmp(cf1->keyspaceName, cf2->keyspaceName) == 0 &&
			strcmp(cf1->cfName, cf2->cfName) == 0;
}

void freeCFM(columnFamilyMetadata *cfmd)
{
	if(cfmd){
		if(cfmd->keyspaceName) free(cfmd->keyspaceName);
		if(cfmd->cfName) free(cfmd->cfName);
		free(cfmd);
	}
}
static void keyDestructor(void *privdata, void *key)
{
	columnFamilyMetadata *cfmd = (columnFamilyMetadata *)key;
	if(cfmd)
		freeCFM(cfmd);
}

/* key is columnFamilyMetadata, value is NULL */
dictType cfmType = {cfmdHash, NULL, NULL, cfmdCompare, 
					keyDestructor,NULL}; 

/*我目前的设计是只在启动的时候创建cfmdict，其他的时候，这个结构体是不变的
  只有当系统退出是，才尝试释放申请的内存*/
dict *g_cfmDict = NULL;
int __init initDictCFMD()
{
	if(g_cfmDict) return -1;

	g_cfmDict = dictCreate(&cfmType, NULL);

	if(g_cfmDict) return 0;

	return -1;
}
int putCFMDToTable(columnFamilyMetadata *cfmd)
{
	if(!cfmd || !g_cfmDict){
		return -1;
	}
	if(dictAdd(g_cfmDict, (void *)cfmd, NULL)  == DICT_ERR){
		return -1;
	}
	return 0;
}
int putMetadataToCFMD(char *keyspaceName, char *cfName, 
							char *type, int cfID)
{
	columnFamilyMetadata *cfmd = NULL;

	if(!(cfmd = (columnFamilyMetadata *)calloc(sizeof(columnFamilyMetadata)
					,1)))
		return -1;

	if(!(cfmd->keyspaceName = strdup(keyspaceName)) ||
				!(cfmd->cfName = strdup(cfName))){
		freeHeapCFMD(cfmd);
		return -1;
	}
	cfmd->type = (strcmp(type, "standard") == 0) ? standard : super;
	cfmd->cfID = cfID;

	if(putCFMDToTable(cfmd) < 0){
		freeHeapCFMD(cfmd);
		return  -1;
	}
	return 0;
}
columnFamilyMetadata *findCFMetaData(char *keyspaceName, char *cfName)
{
	if(!keyspaceName || !cfName || !g_cfmDict) 
		return  NULL;

	columnFamilyMetadata cfm;
	cfm.keyspaceName = keyspaceName;
	cfm.cfName = cfName;

	dictEntry *de = dictFind(g_cfmDict, &cfm);
	if(!de) return NULL;
	
	return (columnFamilyMetadata*)dictGetEntryKey(de);
}

void freeHeapCFMD(columnFamilyMetadata *cfm)
{
	freeCFM(cfm);
}
void freeHeapAllCFMD()
{
	if(g_cfmDict){
		dictRelease(g_cfmDict);
	}
	g_cfmDict = NULL;

}

int freeFromTable(char *keyspaceName, char *cfName)
{
	if(!keyspaceName || !cfName || !g_cfmDict)
		return -1;
	columnFamilyMetadata cfmd;
	cfmd.keyspaceName = keyspaceName;
	cfmd.cfName = cfName;

	if(dictDelete(g_cfmDict,(void *)&cfmd) == DICT_ERR)
		return -1;

	return 0;
}	


int getCFCount(int *count)
{
	int ret = 0;
	if(!g_cfmDict || !count) return -1;
	ret = dictSize(g_cfmDict);
	*count = ret;
	return 0;
}


//下面是文件路径的定义和操作
fileLocation fl;
int getDataPath(const char **dp)
{
	if(!dp || !fl.dataPath) return -1;
	*dp = fl.dataPath;
	return 0;
}
int putDataPath(const char *p)
{
	if(!p)
		return  -1;
	if(!(fl.dataPath = strdup(p)))
		return -1;
	return 0;
}
int putPidPath(const char *p)
{
	if(!p)
		return  -1;
	if(!(fl.pidPath= strdup(p)))
		return -1;
	return 0;
}
int getPidPath(const char **pp)
{
	if(!pp || !fl.pidPath) return -1;
	*pp = fl.pidPath;
	return 0;
}
time_t getCurTime()
{
	return time(0);
}

seed g_sd;
int putSeed(char *seed)
{
	if(!seed) return -1;
	if(g_sd.seedCount >= g_sd.size){
		if(!(g_sd.p = (char **)realloc(g_sd.p, 
						sizeof(char *)*(g_sd.size + 10))))
			return -1;
		g_sd.size += 10;	
	}
	if(!(g_sd.p[g_sd.seedCount++] = strdup(seed)))
			return -1;
	return 0;
}

int deallocSeed()
{
	int i;
	for(i = 0; i< g_sd.seedCount; i++)
		free(g_sd.p[i]);
	free(g_sd.p);
	return 0;
}
void printTableMetadata()
{
	dictIterator *di = NULL;
	dictEntry *de = NULL;
	int i;

	if(!(di = dictGetIterator(g_cfmDict))){
		LOG_WRITE("oom");
		return ;
	}

	de = dictNext(di);
	while(de){
		columnFamilyMetadata *cfmd = (columnFamilyMetadata *)dictGetEntryKey(de);
		LOG_WRITE("keyspaceName:%s cfName:%s type:%s cfID:%d\n", cfmd->keyspaceName,
				cfmd->cfName, cfmd->type == standard ? "standard" : "super", cfmd->cfID);
		de = dictNext(di);
	}

	dictReleaseIterator(di);

	LOG_WRITE("datapath:%s pidpath:%s\n", fl.dataPath, fl.pidPath);

	for(i = 0; i < g_sd.seedCount; i++){
		LOG_WRITE("%d seed:%s\n", i, g_sd.p[i]);
	}
}
