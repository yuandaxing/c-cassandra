#include "bloomFilter.h"
#include "serialize.h"

//murmur hash
int hash(char *data, int length, int seed) 
{
	int m = 0x5bd1e995;
	int r = 24;
	int i;
	int h = seed ^ length;
	int len_4 = length >> 2;
	
	for (i = 0; i < len_4; i++) 
	{
		int i_4 = i << 2;
		int k = data[i_4 + 3];
		k = k << 8;
		k = k | (data[i_4 + 2] & 0xff);
		k = k << 8;
		k = k | (data[i_4 + 1] & 0xff);
		k = k << 8;
		k = k | (data[i_4 + 0] & 0xff);
		k *= m;
		k ^= (k >> r) & 0xff;
		k *= m;
		h *= m;
		h ^= k;
	}
	
	// avoid calculating modulo
	int len_m = len_4 << 2;
	int left = length - len_m;
	
	if (left != 0) {
		if (left >= 3) {
		h ^= (int) data[length - 3] << 16;
		}
		if (left >= 2) {
		h ^= (int) data[length - 2] << 8;
		}
		if (left >= 1) {
			h ^= (int) data[length - 1];
		}
	
		h *= m;
	}
	
	h ^= (h >> 13) & 0x7ff;
	h *= m;
	h ^= (h >> 15) & 0x1ff;
	
	return h;
}


int getBloomFilter(int hashCount, int strCount, bloomFilter **bfv)
{
	bloomFilter *bf = NULL;
	int bitsCount = (hashCount * strCount / 8 + 1) * 8;

	if(!(bf = (bloomFilter *)calloc(sizeof(bloomFilter), 1))){
		return -1;
	}

	//这里的问题是使用calloc出错
	if(!(bf->filterBit = (char *)calloc(bitsCount / 8, 
					1))){
		freeHeapBF(bf);
		return -1;
	}
	bf->strCount = strCount;
	bf->hashCount = hashCount;
	bf->bitsCount = bitsCount;
	*bfv = bf;
	return 0;
}

void freeHeapBF(bloomFilter *bf)
{
	if(!bf) return;
	if(bf->filterBit) free(bf->filterBit);
	free(bf);
}

int serializeBF(buffer *buff, bloomFilter *bf)
{
	if(!buff || !bf || !bf->filterBit) return -1;

	if(writeInt(bf->hashCount, buff) < 0 ||
			writeInt(bf->strCount, buff) < 0 ||
			writeInt(bf->bitsCount, buff) < 0 ||
			writeBytes(bf->filterBit, bf->bitsCount / 8, buff) < 0)
		return -1;

	return 0;
}


int desrializeBF(buffer *buff, bloomFilter **bfv)
{
	bloomFilter *bf = NULL;
	if(!buff) return -1;
	
	if(!(bf = (bloomFilter *)calloc(sizeof(bloomFilter), 
					1))){
		return -1;
	}
	
	if(readInt(buff, &bf->hashCount) < 0 ||
			readInt(buff, &bf->strCount) < 0 ||
			readInt(buff, &bf->bitsCount) < 0 ||
			readBytes(buff, &bf->filterBit, NULL) < 0){
		freeHeapBF(bf);
		return -1;
	}
	if(bfv){
		*bfv = bf;
	}else{
		freeHeapBF(bf);
	}
	return 0;
}

//这一部分是仿照cassandra-java写的
int addToBF(bloomFilter *bf, char *bytes, int len)
{
	int i;
	int hash1, hash2;
	int entry;

	if(!bf||!bytes) return -1;

	hash1 = hash(bytes, len, 0);
	hash2 = hash(bytes, len, hash1);

	for(i = 0; i < bf->hashCount; i++){
		entry = abs((hash1 + i*hash2) % bf->bitsCount);
		bf->filterBit[entry / 8] |=  1 << (entry % 8);
	}
	return 0;
}
bloomFilter *dupBloomFilter(bloomFilter *bf)
{
	bloomFilter *tmp = NULL;

	if(!bf || !bf->filterBit)
		return NULL;
	if(!(tmp = (bloomFilter *)calloc(sizeof(bloomFilter), 1)))
		return NULL;
	if(!(tmp->filterBit = (char *)calloc(bf->bitsCount / 8, 
					1))){
		freeHeapBF(tmp);
		return NULL;
	}
	memcpy(tmp->filterBit, bf->filterBit, bf->bitsCount / 8);
	tmp->hashCount = bf->hashCount;
	tmp->strCount = bf->strCount;
	tmp->bitsCount = bf->bitsCount;
	return tmp;
}
