#ifndef _AUXILFILE_H_
#define _AUXILFILE_H_
#include "buffer.h"

//我们将采用setbuffer函数为file设置自定义的buffer
typedef struct fileAbstract{
	char *fileName;
	FILE *fp;
	char *buff;
	int buffSize;
}fileAbstract;
//返回-1表示打开文件失败，0表示打开文件成功
int openReadFile(char *fileName, int bufferSize, fileAbstract **fa);
int openWriteFile(char *fileName, int bufferSize, fileAbstract **fa);
int freeHeapFile(fileAbstract *fa);
int fseekToPos(fileAbstract *fa, off_t pos);
/*返回写入的字节数*/
int writeAppendFile(fileAbstract *fa, buffer *buff);
//返回读入的字节数，当发生错误，返回-1
int readFile(fileAbstract *fa, buffer *buff);
int synFile(fileAbstract *fa);

long getCurFilePos(fileAbstract *fa);
int endOfFile(fileAbstract *fa);
int openReaderFileFromExist(fileAbstract *fa, int buffSize, 
		fileAbstract **reader);
#endif
