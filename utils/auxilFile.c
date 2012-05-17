#include "auxilFile.h"
#include "serialize.h"
#include "stdio.h"



static int openFile(char *fileName, char *mode, int bufferSize, 
		fileAbstract **fa)
{
	char *buffer = NULL;
	FILE *fp = NULL;
	fileAbstract *file = NULL;
	if(!fileName || !fa) return -1;

	if(!(fp = fopen(fileName,mode))){ 
		return -1;
	}

	if(!(buffer = (char *)malloc(bufferSize))){
	   fclose(fp);
   	   return -1;
	}

	if(!(file = (fileAbstract *)malloc(sizeof(fileAbstract)))){
		free(buffer);
		fclose(fp);
		return -1;
	}

	setbuffer(fp, buffer, bufferSize);

	file->fileName = strdup(fileName);
	file->fp = fp;
	file->buff = buffer;
	file->buffSize = bufferSize;
	*fa = file;
	return 0;
}


int openReadFile(char *fileName, int bufferSize, fileAbstract **fa)
{
	return openFile(fileName, "rb", bufferSize, fa);	
}
int openWriteFile(char *fileName, int bufferSize, fileAbstract **fa)
{
	return openFile(fileName, "wb", bufferSize, fa);
}
int freeHeapFile(fileAbstract *fa)
{
	if(!fa) return -1;
	
	//关闭之前会fflush fa中的数据
	if(fa->fp) fclose(fa->fp);
	if(fa->buff) free(fa->buff);
	if(fa->fileName) free(fa->fileName);

	free(fa);
	return 0;
}
int fseekToPos(fileAbstract *fa, off_t pos)
{
	if(!fa || !fa->fp) return -1;

	return fseek(fa->fp, pos, SEEK_SET);
}
//这里我们将每一个buffer作为一个block，每次写入block前，写入block的大小
//这里必须保证每次写入一个block
int writeAppendFile(fileAbstract *fa, buffer *buff)
{
	char buffSZ[4] = {0};
	int i;

	if(!fa || !fa->fp || !buff) return -1;
	if(buff->buff == NULL) return -1;
	
	for(i = 0; i < 4; i++){
		buffSZ[i] = GET_BYTE(buff->pos, i);
	}
	//注意，这里的返回值为写入item的数量
	if(fwrite(buffSZ, 4, 1, fa->fp) != 1){
		return -1;
	}
	if(fwrite(buff->buff, buff->pos, 1, fa->fp) != 1)
		return -1;
	return 0;
}


//这里保证每次必须读写一个block
int readFile(fileAbstract *fa, buffer *buff)
{
	char buffSZ[4] = {0};
	int readBytes = 0;
	int ret = 0, i;

	if(!fa || !fa->fp || !buff) return -1;

	ret = fread(buffSZ, 4, 1, fa->fp);

	if(ret != 1){
		return -1;
	}
	for(i = 0; i < 4; i++){
		readBytes += PUT_BYTE(buffSZ[i], i);
	}

	if(reserve(buff, readBytes) < 0) return -1;
	if(fread(buff->buff + buff->pos, readBytes, 1, fa->fp) 
							!= 1)
		return -1;
	buff->pos += readBytes;
	return 0;
}

int synFile(fileAbstract *fa)
{
	if(!fa || !fa->fp) return -1;

	return fflush(fa->fp);
}

long getCurFilePos(fileAbstract *fa)
{
	if(!fa || !fa->fp){
		LOG_WRITE("arg error\n");
	   	return -1;
	}
	
	return ftell(fa->fp);
}
int endOfFile(fileAbstract *fa)
{
	if(!fa || !fa->fp)
		return -1;
	return feof(fa->fp);
}
int openReaderFileFromExist(fileAbstract *fa, int buffSize, 
		fileAbstract **reader)
{
	if(!fa || !fa->fileName || !reader)
		return -1;
	return openReadFile(fa->fileName, buffSize, reader);
}
