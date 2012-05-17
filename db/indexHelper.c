#include "indexHelper.h"
#include "serialize.h"

static int serializeKP(buffer *buff, keyPosition *kp)
{
	if(!buff || !kp)
		return -1;
	if(writeBytes(kp->key, strlen(kp->key) + 1, 
				buff) < 0 ||
			writeLong(kp->pos, buff) < 0)
		return -1;

	return 0;
}

static int deserializeKP(buffer *buff, keyPosition *kp)
{
	char *bytes = NULL;
	long l = 0;

	if(!buff || !kp)
		return -1;
	if(readBytes(buff, &bytes, NULL) < 0 ||
			readLong(buff, &l) < 0)
		return -1;

	kp->key = bytes;
	kp->pos = l;

	return 0;
}

void freeTempHeapKeyIndex(keyIndex *ki)
{
	if(!ki)
		return ;
	if(ki->ki)
		free(ki->ki);

	free(ki);
}

void freeIndexIndex(keyIndex *ki)
{
	int i;

	if(!ki)
		return ;
	
	for(i = 0; i < ki->pos; i++){
		if(ki->ki[i].key)
			free(ki->ki[i].key);
	}

	free(ki->ki);
	free(ki);
}

keyIndex *getKeyIndex(long size)
{
	keyIndex *ki = NULL;

	if(size <= 0)
		return NULL;

	if(!(ki = (keyIndex *)calloc(sizeof(*ki), 1)))
		return NULL;
	
	if(!(ki->ki = (keyPosition *)malloc(
					sizeof(keyPosition) * size))){
		freeTempHeapKeyIndex(ki);
		return NULL;
	}

	ki->size = size;
	ki->pos = 0;

	return ki;
}

int serializeKI(buffer *buff, keyIndex *ki, long start, int count)
{
	int i;

	if(!buff || !ki || ki->pos - start < count)
		return -1;

	if(writeInt(count, buff) < 0)
		return -1;

	for(i = 0; i < count; i++){
		if(serializeKP(buff, &ki->ki[start++]) < 0)
			return -1;
	}

	return 0;
}

int deserializeKI(buffer *buff, keyIndex *ki)
{
	int i;
	int count;

	if(!buff || !ki)
		return -1;
	if(readInt(buff, &count) < 0 || ki->size - ki->pos < count)
		return -1;

	for(i = 0; i < count; i++){
		if(deserializeKP(buff, &ki->ki[ki->pos++]) < 0)
		   return -1;	
	}

	return 0;
}
int addIndexNotCopy(keyIndex *ki, char *key, int pos)
{
	if(!ki || !key || ki->pos >= ki->size )
		return -1;
	
	ki->ki[ki->pos].key = key;
	ki->ki[ki->pos].pos = pos;
	ki->pos++;
	return 0;
}

keyIndex *doIndexIndex(keyIndex *ki, ssTable *sst)
{
	long size = 0, i, pos;
	keyIndex *index = NULL;
	char *key = NULL;
	buffer *buff = NULL;

	if(!ki || ki->pos != ki->size)
		return NULL;

	size = ki->size / INDEXINTERVAL + 2;

	if(!(index = getKeyIndex(size)) ||
			!(buff = getBuffer()))
		return NULL;

	for(i = 0; i < ki->pos;	i += INDEXINTERVAL){
		size = INDEXINTERVAL;
		if(ki->pos - i < size)
			size = ki->pos - i;

		if(serializeKI(buff, ki, i, size) < 0 ||
				!(key = strdup(ki->ki[i].key)) ||
					(pos = writeSSTableIndex(buff, sst)) < 0  ||
					addIndexNotCopy(index, key, pos) < 0){
			freeIndexIndex(index);
			freeHeapBuffer(buff);
			//the memory key hold may be leak
			return NULL;
		}	   
	}
	
	freeHeapBuffer(buff);
	return index;	
}
/*
keyPosition *keyFindIndexIndex(char *key, keyIndex *indexIndex)
{
	int s = 0, e;

	if(!key || !indexIndex)
		return NULL;

	e = indexIndex->pos - 1;
	while(e >= s){
		int mid = (e + s) / 2;
		int ret = strcmp(key, indexIndex->ki[mid].key);
		if(ret == 0)
			return &indexIndex->ki[mid];
		if(ret < 0)
			e = mid - 1;
		else
			s = mid + 1;
	}
	
	if(s >= index->pos && e == 0)
		return NULL;




}*/
