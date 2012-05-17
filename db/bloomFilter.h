#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_
#include "stdlib.h"
#include "buffer.h"


#define MINHASH					2
#define KEYHASHCOUNT 			16
#define COLUMNNAMEHASHCOUNT		4

typedef struct bloomFilter{
	int hashCount;
	int strCount;
	int bitsCount;
	char *filterBit;
}bloomFilter;


int getBloomFilter(int hashCount, int strCount, bloomFilter **bf);
void freeHeapBF(bloomFilter *bf);
int serializeBF(buffer *buff, bloomFilter *bf);
int desrializeBF(buffer *buff, bloomFilter **bf);
int addToBF(bloomFilter *bf, char *bytes, int len);
bloomFilter *dupBloomFilter(bloomFilter *bf);
#endif
