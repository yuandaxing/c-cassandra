#include "buffer.h"
#include "ssTable.h"

#define INDEXINTERVAL		128
typedef struct keyPosition{
	char *key;
	long pos;
}keyPosition;

typedef struct keyIndex{
	long size;
	long pos;
	keyPosition *ki;
}keyIndex;
int serializeKI(buffer *buff, keyIndex *ki, long start, int count);
int deserializeKI(buffer *buff, keyIndex *ki);

void freeTempHeapKeyIndex(keyIndex *ki);
void freeIndexIndex(keyIndex *ki);
keyIndex *getKeyIndex(long size);
int addIndexNotCopy(keyIndex *ki, char *key, int pos);
keyIndex *doIndexIndex(keyIndex *ki, ssTable *sst);
keyPosition *keyFindIndexIndex(char *key, keyIndex *indexIndex);


