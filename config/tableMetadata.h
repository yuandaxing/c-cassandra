#ifndef _TABLEMETADATA_H_
#define _TABLEMETADATA_H_
#include "stdio.h"
#include "time.h"
#include "init.h"
#include "dict.h"



enum cfType{standard, super};
/*当前只设计column */
typedef struct columnFamilyMetadata{                     
    char *keyspaceName;                          
    char *cfName;                               
	enum cfType type;
	int cfID;
}columnFamilyMetadata; 

extern dict *g_cfmDict;
//这里使用hash表组织metadata的使用
unsigned int cfmdHash(const void *key);
int cfmdCompare(void *privdata, const void *key1, const void *key2);


int initDictCFMD();
int putCFMDToTable(columnFamilyMetadata *cfm);
int putMetadataToCFMD(char *keyspaceName, char *cfName, char *type, int cfID);
columnFamilyMetadata *findCFMetaData(char *keyspaceName, char *cfName);
void freeHeapCFMD(columnFamilyMetadata *cfm);
void freeHeapAllCFMD();
int freeFromTable(char *keyspaceName, char *cfName);
int getCFCount(int *count);


//这里使用的时候将路径标准化，最后不带"/"
typedef struct fileLocation{
	char *dataPath;
	char *pidPath;
	char *configPath;
	char *otherPath;
}fileLocation;

int getDataPath(const char **dp);
int putDataPath(const char *p);
int putPidPath(const char *p);
int getPidPath(const char **pp);
time_t getCurTime();

typedef struct seed{
	char **p;
	int seedCount;
	int size;
}seed;

extern seed g_sd;
int putSeed(char *seed);
int deallocSeed();

//下面主要提供一些 debug的函数 

void printTableMetadata();
#endif
