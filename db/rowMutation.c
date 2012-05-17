#include "rowMutation.h"


rowMutation *getRM()
{
	rowMutation *rm = NULL;
	if(!(rm = (rowMutation*)malloc(sizeof(rowMutation)))){
		LOG_WRITE("oom\n");
		return NULL;
	}

	
	*rm = (rowMutation){NULL, NULL, NULL, NULL}; 
	return rm;
}


void freeHeapRM(rowMutation *rm)
{
	if(!rm) return ;
	if(rm->keyspaceName) free(rm->keyspaceName);
	if(rm->key) free(rm->key);
	if(rm->cfName) free(rm->cfName);
	if(rm->cf) freeHeapColumnFamily(rm->cf);
	free(rm);
}

int serializeRM(buffer *b, rowMutation *rm)
{
	if(!b || !rm || !rm->cf) 
		return -1;
	//我们保存的是二进制流，所有将字符\0也写入到文件，这样反序列化的时候
	//比较好处理
	if(writeBytes(rm->keyspaceName, strlen(rm->keyspaceName) + 1,  b) < 0 ||
			writeBytes(rm->key, strlen(rm->key) + 1, b) < 0 ||
			writeBytes(rm->cfName, strlen(rm->cfName) + 1, b) < 0 ||
			serializeCF(rm->cf, b) < 0){
		return -1;
	}
	return 0;
}
int deserializeRM(buffer *b, rowMutation *rm)
{
	char *key = NULL, *cfName = NULL, *keyspaceName = NULL;
	
	if(!rm) 
		return -1;

	if(readBytes(b, &keyspaceName, NULL) < 0 ||
			readBytes(b, &key, NULL) < 0 ||
			readBytes(b, &cfName, NULL) < 0){
			if(key) free(key);
		if(cfName) free(cfName);
		if(keyspaceName) free(keyspaceName);
		return -1;
	}

	rm->key = key;
	rm->cfName = cfName;
	rm->keyspaceName = keyspaceName;

	columnFamilyMetadata *cfm = findCFMetaData(keyspaceName, cfName);

	if(!cfm){
		return -1;
	}
	columnFamily *cf = NULL;
	if(!(cf = getCF(cfm))) return -1;	

	rm->cf = cf;
	if(deserializeCF(b, rm->cf) < 0){
		return -1;
	}
	return 0;
}


int getRMCFID(rowMutation *rm, int *id)
{
	if(!rm || !rm->cf)
		return -1;
	return getCFID(rm->cf, id);
}
/*int insertRM(rowMutation *rm)
{
	if(!rm){
		return -1;
	}

	return 0;
}*/
