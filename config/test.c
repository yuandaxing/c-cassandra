#include "config.h"
#define TEST_LEN  10

char line[TEST_LEN][100] = 
{
	"1this is really helpful",
	"2seed: 192.168.1.1", 
	"3sssssssssss  \n",
	"4ddddddd sssss",
	" 5 huo:ying",
	" 6 ss: kk ",
	"  #7help",
	"#8test",
	"9sss:  eee\t ccc",
	"   "
};

int  leak()
{
	char *p = NULL;
	p = (char *)malloc(sizeof(20));
	return -1;
}
int main()
{
	leak();
	//int i;
//	for(i = 0; i < TEST_LEN && line[i]; i++){
//		printf("%d:%s\n", i, line[i]);
//		strTrim(line[i], strlen(line[i]), " \t\n");
//		printf("%d:%s\n", i, line[i]);
//	}
//	tokens tks = {0, 0, 0};
//	for(i = 0; i < TEST_LEN && line[i] != NULL; i++){
//		if(shouldSkip(line[i], strlen(line[i]))){
//			printf("line[%d]:%s should skip\n", i, line[i]);
//		}
//		tks.tkCount = 0;
//		splite(line[i], ':', &tks, " \t\n");
//		printTokens(&tks);
//	}
	initDictCFMD();
	loadConfig("./cassandra.conf");
	printTableMetadata();
	return 0;
}



















