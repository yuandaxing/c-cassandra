#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "init.h"
#include "tableMetadata.h"

typedef struct token{
    char *start;
    int len;
}token;

typedef struct tokens{
    token *tk;
    int tkCount;
    int tkSize;
}tokens;

int isComment(char *line, int len);
int shouldSkip(char *line, int len);
int strTrim(char *line, int len, char *characters);
int reserveTK(tokens *tks, int size);
int deallocTK(tokens *tks);
int splite(char *line, char delimiter, tokens *tks, char *tokeDelimter);
//该接口主要是用来测试
void printTokens(tokens *tks);

/*该问题的设计方法如下：通过一个链表注册相应的key，处理方法，然后每次读取一行
  ，首先判断出是否是注释行，如果不是，则分离出相应的key，然后在链表中查找相应的方法处理.
这里为了方便使用，我们之间使用数组表示就可以了*/
typedef int (*Proc)(tokens *tks);
typedef struct keyProc{
    char *name;
    Proc proc;
}keyProc;


//该函数主要是读取每一行，然后分发给相应的函数处理
int __init loadConfig(char *confFile);

