#ifndef _QUERYPATH_H_
#define _QUERYPATH_H_
#include "stdio.h"
#include "serialize.h"

typedef struct queryPath{
	char *keyspace;
	char *cfName;
	char *key;
	char *superColumnName;
	char *columnName;
	unsigned int before;
}queryPath;

void freeHeapQP(queryPath *qp);

int serializeQPath(buffer *buff, queryPath *qp);
int deserializeQPath(buffer *buff, queryPath **qp);

#endif
