#include "queryPath.h"

void freeHeapQP(queryPath *qp)
{
	if(qp)
		return ;

	if(qp->keyspace)
		free(qp->keyspace);
	if(qp->cfName)
		free(qp->cfName);
	if(qp->key)
		free(qp->key);
	if(qp->superColumnName)
		free(qp->superColumnName);
	if(qp->columnName)
		free(qp->columnName);

	free(qp);
}



