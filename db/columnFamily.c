#include "columnFamily.h"

int initCF(columnFamily *cf, columnFamilyMetadata *cfmd)
{
	if(!cfmd){
		LOG_WRITE("cfmd cannot NULL\n");
		return -1;
	}
	if(!cf) return -1;

	*cf = (columnFamily){NULL, 0, 0, cfmd };
	if(cfmd->type == standard){
		if(!(cf->columnorSuper = getColumnSet())) return -1;
		return 0;
	}else{
		//here we do not implement superColumnFamily
		return -1;
	}
}

columnFamily* getCF(columnFamilyMetadata *cfm)
{
	columnFamily *cf = NULL;
	if(!cfm){
		return NULL;
	}
	cf = (columnFamily *)malloc(sizeof(columnFamily));
	if(!cf) 
		return NULL;
	
	*cf = (columnFamily){NULL, 0, 0, cfm};
	if(cfm->type == standard){
		if(!(cf->columnorSuper = getColumnSet())){
			freeHeapColumnFamily(cf);
			return NULL;
		}
		return cf;
	}else{
		//currently we do not implement superColumn
		return NULL;
	}
}

int getCFType(columnFamily *cf, enum cfType *v)
{
	if(!cf || !cf->cfm) return -1;

	if(v)
		*v = cf->cfm->type;
	return 0;
}

char *getCFName(columnFamily *cf)
{
	if(!cf || !cf->cfm) 
		return NULL;

	return cf->cfm->cfName;
}
char *getCFKeyspace(columnFamily *cf)
{
	
	if(!cf || !cf->cfm) 
		return NULL;

	return cf->cfm->keyspaceName;
}

int getCFID(columnFamily *cf, int *ID)
{
	if(!cf || !cf->cfm) return -1;

	if(ID)
		*ID= cf->cfm->cfID;
	return 0;
}
int getCFColumnCount(columnFamily *cf, int *count)
{
	enum cfType type;

	if(!cf || !cf->columnorSuper) 
		return -1;
	if(getCFType(cf, &type) < 0) 
		return -1;

	if(type == standard){
		if(getColumnCount((columnSet*) cf->columnorSuper, 
							count) < 0)
		   return -1;	

		return 0;
	}else{

		return -1;
	}
}

static int cfCompare(columnFamily *cf1, columnFamily *cf2)
{
	char *ksName1 = NULL, *ksName2 = NULL;
	char *cfName1 = NULL, *cfName2 = NULL;
	int com;

	ksName1 = getCFKeyspace(cf1);
	ksName2 = getCFKeyspace(cf2);
	cfName1 = getCFName(cf1);
	cfName2 = getCFName(cf2);
	
	com = strcmp(ksName1, ksName2);
	if(com) 
		return com;
	else
		return strcmp(cfName1, cfName2);
}

int serializeCF(columnFamily *cf, buffer *buff)
{
	if(!cf || !buff) return -1;

	enum cfType type;

	if(getCFType(cf, &type) < 0) return -1;

	if(writeUnsignedInt(cf->markedForDelete, buff) < 0 ||
			writeUnsignedInt(cf->localDelete, buff) < 0)
		return -1;
	if(type == standard){
		return serializeColumnSet((columnSet *)cf->columnorSuper, buff);
	}else{
		return -1;
	}
}

int deserializeCF(buffer *buff, columnFamily *cf)
{
	enum cfType type;
	unsigned int markedForDelete, localDelete;

	if(!buff || !cf) 
		return -1;

	if(getCFType(cf, &type) < 0) 
		return -1;

	if(readUnsignedInt(buff, &markedForDelete) < 0 ||
			readUnsignedInt(buff, &localDelete) < 0)
		return -1;

	cf->markedForDelete = markedForDelete;
	cf->localDelete = localDelete;
	if(type == standard){
		return deserializeColumnSet(
				(columnSet *)cf->columnorSuper, buff);
	}else{
		return -1;
	}
}
//返回添加的column的数量, columnFamily的删除时间应该取两个
//中的较大值
int insertCF(columnFamily *cf, columnFamily *inserted)
{
	int insert = 0;
	enum cfType type;

	if(!cf || !inserted ||
			!cf->cfm || !inserted->cfm) 
		return -1;
	
	//这里确认两个columnFamily的metadata必须一致才能
	//将一个cf中的column插入到另一个columnFamily
	if(cfCompare(cf, inserted))
		return -1;

	if(getCFType(cf, &type) < 0) 
		return -1;

	cf->markedForDelete = MAX(cf->markedForDelete, 
			inserted->markedForDelete);
	cf->localDelete = MAX(cf->localDelete, 	
			inserted->localDelete);

	if(type == standard){
		insert = addColumnSet(
				(columnSet *)cf->columnorSuper, 
				(columnSet *)inserted->columnorSuper);

		return insert;
	}else{

		return -1;
	}
}

column *findCFColumn(columnFamily *cf, char *columnName)
{
	enum cfType type;

	if(!cf || !columnName || !cf->columnorSuper)
		return NULL;
	if(getCFType(cf, &type) < 0)
		return NULL;

	if(type == standard){
		return findCSColumn((columnSet *)cf->columnorSuper, 
				columnName);
	}else{
		return NULL;
	}
}

void clearColumnFamily(columnFamily *cf)
{
	if(!cf) return;
	//表明该cf不是一个合法的cf
	cf->cfm = NULL;
	freeHeapColumnSet(cf->columnorSuper);
	cf->columnorSuper = NULL;
}
void freeHeapColumnFamily(columnFamily *cf)
{
	if(!cf) return ;
	clearColumnFamily(cf);
	free(cf);
}

int addCFColumn(columnFamily *cf , column *toInsert)
{
	enum cfType type;

	if(!cf || !cf->columnorSuper || !toInsert){
		return -1;
	}
	if(getCFType(cf, &type) < 0)
		return -1;
	if(type == standard)
		return addColumn((columnSet *)cf->columnorSuper, toInsert);
	else
		return -1;
}


