#include "config.h"
#include "log.h"
#include "tableMetadata.h"


//1表明是注释，0表示不是
int isComment(char *line, int len)
{
	if(line[0] == '#')
		return 1;
	return 0;
}

int shouldSkip(char *line, int len)
{
	if(line[0] == '\0' || len == 0 ||
			isComment(line, len))
		return 1;
	return 0;
}

int strTrim(char *line, int len, char *characters)
{
	int i = 0, pos;
	char *p = NULL;
	if(!line || !characters) return -1;
	for(i = 0; i < len; i++){
		p = strchr(characters, line[i]);
		if(!p) break;
	}
	if(i > len){
		line[0] = '\0';
		return 0;
	}
	pos = 0;
	for(;i < len; i++){
		line[pos++] = line[i];
	}
	line[pos] = 0;

	pos--;
	for(; pos >= 0; pos--){
		p = strchr(characters, line[pos]);
		if(p){
			line[pos] = '\0';
		}else{
			break;
		}
	}
	return 0;
}
int reserveTK(tokens *tks, int size)
{
	int newsize;
   	token *t;
	if(!tks) return -1;
	if(tks->tkSize - tks->tkCount > size) return 0;

	newsize = tks->tkSize + 2 *size;
	t = (token *)realloc(tks->tk, newsize *(sizeof(token)));
	if(t){
		tks->tk = t;
		tks->tkSize = newsize;
		return 0;
	}
	return -1;
}
int deallocTK(tokens *tks)
{
	if(tks->tk) free(tks->tk);
	tks->tk = NULL;
	return 0;
}
int splite(char *line, char delimiter, tokens *tks, char *tokenDelimter)
{
	int tkpos, len;
	char *p = NULL, *stk = NULL, *next = NULL;
	if(!line || !tks || !tokenDelimter){
		return -1;
	}
	tks->tkCount = 0;
	
	//获取key
	p = strchr(line, delimiter);
	if(!p){
		return -1;
	}
	if(reserveTK(tks, 1) < 0) return -1;
	next = p + 1; 
	*p = '\0';
	stk = line;
	strTrim(stk, strlen(stk), tokenDelimter);
	tkpos = tks->tkCount;
	tks->tkCount++;
	tks->tk[tkpos].start = stk;
	tks->tk[tkpos].len = strlen(stk);
	//处理相应的value字段
	//printf("next %s", next);
	while(1){
		strTrim(next, strlen(next), tokenDelimter);
		stk = next;
		len = strlen(stk);
		while(next < len + stk){
			if((p = strchr(tokenDelimter, *next)
			   			)!= NULL){
					*next = '\0';
					next++;
					break;
				}
			next++;
		}
		if(next == stk) break;
		if(reserveTK(tks, 1) < 0) return -1;
		tkpos = tks->tkCount;
		tks->tkCount++;
		tks->tk[tkpos].start = stk;
		tks->tk[tkpos].len = strlen(stk);
	}

	return 0;
}


void printTokens(tokens *tks)
{
	int i;
	for(i = 0; i < tks->tkCount; i++){
		printf(" token %d: %s\n", i, tks->tk[i].start);
	}
}

static int seedProc(tokens *tks)
{
	int i;

	for(i = 1; i < tks->tkCount; i++){
		if(putSeed(tks->tk[i].start) < 0){
			LOG_WRITE("set seed  error\n");
			return -1;
		}

		//printf("seed:%s\n", tks->tk[i].start);
	}
	return 0;
}
static char curKeyspace[100] = {0};
static int keyspaceProc(tokens *tks)
{
	if(tks->tkCount < 2) return -1;
	strcpy(curKeyspace, tks->tk[1].start);

	//LOG_WRITE("keyspace is %s\n", tks->tk[1].start);

	return 0;
}
static int curCFID = 0;
static int cfProc(tokens *tks)
{

	if(!tks)
		return -1;
	if(tks->tkCount < 3) 
		return -1;
	if(strlen(curKeyspace) == 0)
		return -1;

	LOG_WRITE("cftype:%s\n", tks->tk[2].start);
	if(putMetadataToCFMD(curKeyspace, tks->tk[1].start, 
				tks->tk[2].start, curCFID) < 0){
		return -1;
	}else{
		curCFID++;
		return 0;
	}
}
static int dataPathPorc(tokens *tks)
{
	int len = 0;
	char *p;
	if(!tks || tks->tkCount < 2)
		return -1;
	len = strlen(tks->tk[1].start);

	while(len-- > 0){
		p = &tks->tk[1].start[len];
		if(*p == '/')
			*p = '\0';
		else
			break;
	}
	if(len >= 0){
		return putDataPath(tks->tk[1].start);
	}else{
		return -1;
	}
}

static int pidPathPorc(tokens *tks)
{
	int len = 0;
	char *p;
	if(!tks || tks->tkCount < 2)
		return -1;
	len = strlen(tks->tk[1].start);

	while(len-- > 0){
		p = &tks->tk[1].start[len];
		if(*p == '/')
			*p = '\0';
		else
			break;
	}
	if(len >= 0){
		return putPidPath(tks->tk[1].start);
	}else{
		return -1;
	}
}
keyProc  ProcList[] = {
					{"seed", seedProc},
					{"keyspace", keyspaceProc},
					{"columnFamily", cfProc},
					{"datapath", dataPathPorc},
					{"pidpath", pidPathPorc},
				    {0, 0}//indicate the end
					  };



int __init loadConfig(char *confFile)
{

	FILE *fp = NULL;
	if(initDictCFMD() < 0)
		return -1;
	if(!(fp = fopen(confFile, "r"))){
		return -1;
	}

	char buff[1000];

	memset(buff, 0, sizeof(buff));
	while(fgets(buff, 1000, fp) != NULL){
		strTrim(buff, strlen(buff), " \t\n");
		if(shouldSkip(buff, strlen(buff))){
			continue;
		}	


		tokens tks = {0, 0, 0};
		int i;

		if(splite(buff, ':', &tks, " \t\n") == 0){
			for(i = 0; ProcList[i].name != NULL; i++){
				if(strcmp(ProcList[i].name, 
							tks.tk[0].start) == 0){
					ProcList[i].proc(&tks);
					break;//跳出for
				}
			}
		}

		deallocTK(&tks);
		memset(buff, 0, sizeof(buff));
	}

	fclose(fp);
	return 0;
}
