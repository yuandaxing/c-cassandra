#include "stdio.h"
#include "dict.h"
#include "string.h"
#include "stdlib.h"


/*
 * bloomfilter hashtable bitmap三种线性的数据处理
 */
//该实现中缺少一些指针的检查，所以使用之前要确保指针的正确性
unsigned int strHash(const void *key)
{
	char *str = (char *)key;
	return  dictGenHashFunction((const unsigned char *)str, 
													strlen(str));
}
//相等返回true，否则返回false
int keyCompare(void *privdata, const void *key1, const void *key2)
{
	char *str1 = (char *)key1;
	char *str2 = (char *)key2;
	return strcmp(str1, str2) == 0;
}

//keyDup, valueDup主要用于插入的时候复制数据，这一项不需要
void keyDestructor(void *privdata, void *key)
{
	free(key);
}
void valDestructor(void *privdata, void *value)
{
	free(value);
}
dictType strTest = {strHash, NULL, NULL, keyCompare, 
						keyDestructor,valDestructor};

int main()
{

	dict *hashmap = dictCreate(&strTest, NULL);
	int i;
	for(i = 0; i < 100; i++){
		char *str = (char *)malloc(10);
		char *val = (char *)malloc(10);
		char rem = i % 26;
		int j;
		for(j = 0; j < 10; j++){
			str[j] = 'a' + rem;
			val[j] = 'a' + rem;
		}
		str[9] = 0;
		val[9] = 0;
		if(dictAdd(hashmap, str, val) == DICT_ERR){
			//printf("insertd %s err\n", str);
			free(str);
			free(val);
			continue;
		}
		printf("inserted %s\n", str);
		

		

	}
	char *key1= "aaaaaaaab";
	dictEntry *de = dictFind(hashmap, key1);
	if(!de){
		printf("cannot find key %s\n", key1);
		return 0;
	}
	else
		printf("find value %s\n", (char *)dictGetEntryVal(de));
	return 0;
}
