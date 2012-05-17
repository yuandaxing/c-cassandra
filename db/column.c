#include "column.h"

static inline int columnComp(column *x, column *y)
{
	if(!x || !y){
		LOG_WRITE("x and y pointer suppose not NULL\n");
		return 0;
	}
	return strcmp(x->name, y->name);
}

column *getColumn()
{
	column *ret = NULL;

	if((ret = (column *)calloc(sizeof(column), 1)) == NULL){
		LOG_WRITE("out of memory\n");
		return NULL;
	}

	ret->refCount = 1;
	return ret;
}
column * cloneColumn(column *x)
{
	column *y = NULL;
	char *name = NULL, *value = NULL;

	if(!x) 
		return NULL;
	if((y = getColumn()) == NULL) 
		return NULL;
	if(!(name = strdup(x->name)) 
			|| !(value = strdup(x->value))){
		if(name) 
			free(name);
		if(value) 
			free(value);
		decrColumnRef(y);

		return NULL;
	}

	y->name = name;
	y->value = value;
	y->timestamp = x->timestamp;
	y->del = x->del;
	y->size = x->size;

	return y;
}

pthread_mutex_t columnMutex = PTHREAD_MUTEX_INITIALIZER;
void decrColumnRef(column *c)
{
	if(!c) return;

	pthread_mutex_lock(&columnMutex);
	c->refCount--;
	if(!c->refCount){
		freeHeapColumn(c);
	}
	pthread_mutex_unlock(&columnMutex);
}
void incrColumnCount(column *c)
{
	if(!c)
		return ;
	pthread_mutex_lock(&columnMutex);
	c->refCount++;
	pthread_mutex_unlock(&columnMutex);
}
void freeHeapColumn(column *x)
{
	if(!x) return ;

	if(x->name) free(x->name);
	if(x->value) free(x->value);
	free(x);
}

int compPriority(column *x, column *y) 
{
	if(!x || !y){
		LOG_WRITE("x and y should not be NULL\n");
		return 0;
	}
	if(x->del == y->del){
		return x->timestamp - y->timestamp;
	}
	return x->del - y->del;
}
//unused
int getColumnSerializeLen(column *x)
{
	if(!x){
		LOG_WRITE("pointer x should not be NULL\n");
		return 0;
	}

	return x->size + COLUMNSEREX;
}


int serializeColumn(column *x, buffer *buff)
{
	if(!x || !buff){
		LOG_WRITE("column and buffer should not be NULL\n");
		return -1;
	}

	if(writeBytes(x->name, strlen(x->name) + 1, buff) < 0 
			|| writeBytes(x->value, strlen(x->value) + 1, 
				buff) < 0 
			|| writeUnsignedInt(x->timestamp, buff) < 0 
			|| writeByte(x->del, buff) < 0) 
		return -1;
	return 0;
}


int deserializeColumn(buffer *buff, column **x)
{
	unsigned int timestamp;
	char del;
	char *name = NULL, *value = NULL;
	column *v = NULL;

	if(readBytes(buff, &name, NULL) < 0 ||
			readBytes(buff, &value, NULL) < 0 ||
			readUnsignedInt(buff, &timestamp) < 0 ||
			readByte(buff, &del) < 0 ){
		if(name) free(name);
		if(value) free(value);
		return -1;
	}

	if(!(v = getColumn())){
		if(name) 
			free(name);
		if(value) 
			free(value);

		return -1;
	}
	v->name = name;
	v->value = value;
	v->timestamp = timestamp;
	v->del = del;

	//获取column或者释放column 
	if(x) 
		*x = v;
	else 
		decrColumnRef(v);

	return 0;
}

RB_PROTOTYPE(columnsHeader, column, link, columnComp); 
RB_GENERATE(columnsHeader, column, link, columnComp);

columnSet *getColumnSet()
{
	columnSet *cs = NULL;

	if(!(cs = (columnSet *)malloc(sizeof(columnSet)))){
		LOG_WRITE("oo mem\n");
		return NULL;
	}
	*cs = (columnSet){{NULL}, 0};

	return cs;
}

int getColumnCount(columnSet *cs, int *v)
{
	if(!cs) 
		return -1;

	if(v) 
		*v = cs->elemCount;

	return 0;
}
/*
 *因为freeBSD没有提供相关析构的函数，该实现效率很低
 *O(nlog(n))复杂度
 */

void freeHeapColumnSet(columnSet *cs)
{
	column *tmp = NULL;
	if(!cs) return;

	tmp = RB_MIN(columnsHeader, &cs->header);
	while(tmp != NULL){
		RB_REMOVE(columnsHeader, &cs->header, tmp);
		cs->elemCount--;
		decrColumnRef(tmp);
		tmp = RB_MIN(columnsHeader, &cs->header);
	}
	free(cs);	
}

//返回-1，需要释放column
int addColumn(columnSet *cs, column *toInsert)
{
	column *tmp = NULL;

	if(!cs || !toInsert) 
		return -1;

	tmp = RB_FIND(columnsHeader, &cs->header, toInsert);
	if(!tmp){
		RB_INSERT(columnsHeader, &cs->header, toInsert);
		cs->elemCount++;
		return 1;
	}else{
		if(compPriority(toInsert, tmp) > 0){//column数量不变
			RB_REMOVE(columnsHeader, &cs->header, tmp);
			RB_INSERT(columnsHeader, &cs->header, toInsert);
			decrColumnRef(tmp);

			return 0;
		}
		return -1;
	}
}
//-1表示参数有问题,其他表明插入的个数
//因为并没有提供内置的快速合并红黑树的操作，该算法效率为O(nlog(n+m))
int addColumnSet(columnSet *target, columnSet *src)
{
	int columnAdded = 0, addFlag = 0;
	column *tmp = NULL;

	if(!target){
		return -1;
	}
	if(!src){
		return 0;
	}

	tmp = RB_MIN(columnsHeader, &src->header);
	while(tmp != NULL){
		int ret;

		RB_REMOVE(columnsHeader, &src->header, tmp);
		src->elemCount--;

		ret = addColumn(target, tmp);
		if(ret == -1) 
			decrColumnRef(tmp);
		if(ret > 0){
			addFlag = 1;	
			columnAdded += ret;
		}

		tmp = RB_MIN(columnsHeader, &src->header);
	}
	if(addFlag)
		return columnAdded;
	else
		return -1;
}

int serializeColumnSet(columnSet *cs, buffer *buff)
{
	if(!cs || !buff) return -1;
	if(writeInt(cs->elemCount, buff) < 0){
		LOG_WRITE("oo mem\n");
		return -1;
	}
	column *tmp = NULL;
	RB_FOREACH(tmp, columnsHeader, &cs->header){
		if(serializeColumn(tmp, buff) < 0) return -1;
	}
	return 0;
}

int deserializeColumnSet(columnSet *cs, buffer *b)
{
	int count = 0;
	if(readInt(b, &count) < 0){
		return -1;
	}

	int i;
	column *tmp = NULL;
	for(i = 0; i < count; i++){
		if(deserializeColumn(b, &tmp) < 0){
			return -1;
		}
		
		if(addColumn(cs, tmp) < 0){
			decrColumnRef(tmp);
		}
	}
	return 0;
}
//-1 error -2 not find 0 find and delete
int deleteColumn(columnSet *cs, column *c)
{
	if(!cs || !c){
		return -1;
	}
	column *x = NULL;
	x = RB_FIND(columnsHeader, &cs->header, c);
	if(!x) return -2;
	RB_REMOVE(columnsHeader, &cs->header, x);
	decrColumnRef(x);
	return 0;
}
column *findCSColumn(columnSet *cs, char *name)	
{
	column tmp = {name};
	column *c = NULL;
	
	if(!cs || !name)
		return NULL;
	c = RB_FIND(columnsHeader, &cs->header, &tmp);
	if(c)
		incrColumnCount(c);

	return c;
}
