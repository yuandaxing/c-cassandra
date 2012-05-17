#ifndef _DATAMODEL_H_
#define _DATAMODEL_H_
#include "stdio.h"
#include "rowMutation.h"
#include "columnFamily.h"
#include "init.h"
#include "log.h"
#include "dict.h"
#include "columnFamilyStore.h"
#include "tableMetadata.h"
#include "queryPath.h"

typedef struct dataModel{
	dict *cfsHashTable;
}dataModel;

int __init initDM();
void __exit destroyDM();
int insertDM(rowMutation *rm);
int forceFlushDM(char *keyspaceName, char *cfName);
int doCompactDM(char *keyspaceName, char *cfName);
column *getColumnDM(queryPath *path);
#endif
