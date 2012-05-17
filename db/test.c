#include "auxiRBtree.h"
#include "stdio.h"
#include "column.h"
#include "ssTable.h"
#include "tableMetadata.h"
#include "config.h"
#include "dataModel.h"
#include "columnFamily.h"
#include "commitLog.h"

char *config = "./cassandra.conf";

int main()
{
	rowMutation *rm = NULL;
	columnFamily *cf = NULL;
	column *x = NULL;
	queryPath  qp = {"Test", "cf1", "key0", NULL, "name", 0};
//	char path[1000] = "/home/ydx/test/commitlog/commitlog-1317532982.db";

	if(loadConfig(config) < 0 ){
		LOG_WRITE("cannot load configure ");
		return -1;
	}
	printTableMetadata();
	if(initDM() < 0){
		LOG_WRITE("INIT dataModel error\n");
	}
	//here is the marked
	int i;
	for(i = 0; i < 100000; i++){
		char keytmp[300];

		sprintf(keytmp, "key%d", i);

		if(!(rm = (rowMutation *)calloc(sizeof(rowMutation), 1))){
			return -1;
		}

		rm->keyspaceName = strdup("Test");
		rm->key = strdup(keytmp);
		rm->cfName = strdup("cf1");
		columnFamilyMetadata *cfmd = findCFMetaData(rm->keyspaceName,
				rm->cfName);

		if(!cfmd || !(cf = getCF(cfmd))){
			return -1;
		}	
		cf->localDelete = 2000;
		cf->markedForDelete= 2000;

		x = getColumn();
		if(!x){
			return -1;
		}
		x->name = strdup("name");
		x->value = strdup("ydx");
		x->timestamp = 20000;
		//return -1表明必须释放column的空间,这里为了节省空间
		//不得不这样纠结的设计
		if(addCFColumn(cf, x) < 0){
			LOG_WRITE("cannot add column to cf\n");
			decrColumnRef(x);
			x = NULL;
		}

		rm->cf = cf;

		if(insertDM(rm) < 0){
			LOG_WRITE("insert error\n");
			freeHeapRM(rm);
			continue;
		}
	//	LOG_WRITE("insert succeed\n");
		freeHeapRM(rm);
	}
//	forceFlushDM("Test", "cf1");
	//printf("please input the path to test replay\n");
	//scanf("%s", path); 
//	auxilary_recover(path);
//	column *v = NULL;
//	printf("getColumnDM return %d\n", getColumnDM(&qp, &v));
//	if(v)
//		printf("name :%s", v->value);
//	
//
	return 0;
//	getchar();
}
