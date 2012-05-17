/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:	2011-09-27 14:20
*
* Filename:		commitLog.c
*
* Description: 
*
=============================================================================*/
#include "commitLog.h"
#include "stdlib.h"
#include "time.h"
#include "auxilFile.h"
#include "crc32.h"
#include "rowMutation.h"
#include "dataModel.h"
#include "tableMetadata.h"

extern commitLog *curCL;

/*这里对clheader相关的进行抽象*/
typedef struct clHeader{
    int cfCount;
    int byteCount;
    char *dirtyBit;
    int *posAt;
}clHeader;

/*创建一个size大小的header */
clHeader *getCLHeader(int cfCount);
void freeHeapCLHeader(clHeader *header);
//改变函数
/*设置某一个entry项为脏*/
void setDirty(clHeader *header, int entry);
void clearDirty(clHeader *header, int entry);
/*设置某一项当前的位置*/ 
void setEntryPos(clHeader *header, int entry, int position);
//-1表示出错，0表示干净，1表示脏
//选择函数
int isDirty(clHeader *header, int entry);
int serializeCLH(clHeader *header, buffer *buff);
clHeader *deserializeCLH(buffer *buff);
int getFirstDirtyPos(clHeader *header);

static int writeHeaderSyn(commitLog *cl, int );

clHeader *getCLHeader(int cfCount)
{
	clHeader *clh = NULL;
	char *bytes = NULL;
	int *pos = NULL;

	if(!(clh = (clHeader *)malloc(sizeof(clHeader)))) 
		return NULL;
	if(!(pos = (int *)calloc(sizeof(int), cfCount))){
		free(clh);
	   	return NULL;
	}
	clh->byteCount = cfCount / 8 + 1;
	if(!(bytes = (char *)calloc(sizeof(char), 
					clh->byteCount))){
	   free(clh);
	   free(pos);   	
	   return NULL;
	}

	clh->cfCount = cfCount;
	clh->dirtyBit = bytes;
	clh->posAt = pos;

	return clh;
}

void freeHeapCLHeader(clHeader *header)
{
	if(!header) 
		return ;

	if(header->dirtyBit) 
		free(header->dirtyBit);
	if(header->posAt) 
		free(header->posAt);
	free(header);
}

void setDirty(clHeader *header, int entry)
{
	int rem, be;
	if(!header || !header->dirtyBit){ 
		printf("bad header\n");
		return ;
	}
	//entry从0开始
	if(entry >= header->cfCount || entry < 0){
		LOG_WRITE("entry error\n");
		return ;
	}

	be = entry / 8;
	rem = entry % 8;
	header->dirtyBit[be] |= 1 << rem;

	return ;
}
void setEntryPos(clHeader *header, int entry, int position)
{
	if(!header || header->posAt == NULL) 
		return ;
	if(entry >= header->cfCount || entry < 0){
		LOG_WRITE("entry error\n");
		return ;
	}
	header->posAt[entry] = position;

	return ;
}
int isDirty(clHeader *header, int entry)
{
	int be, re;

	if(!header || !header->dirtyBit){ 
		LOG_WRITE("we cannot offord argument errorn\n");
		return 0;
	}
	if(entry >= header->cfCount || entry < 0){
		LOG_WRITE("entry error\n");
		return 0;
	}
	be = entry / 8;
	re = entry % 8;

	return (header->dirtyBit[be] & (1 << re)) > 0 ?
				1 : 0;
}
//清除dirty位
void clearDirty(clHeader *header, int entry)
{
	int rem, be;

	if(!header || !header->dirtyBit){ 
		LOG_WRITE("header error\n");
		return ;
	}
	if(entry >= header->cfCount || entry < 0){
		LOG_WRITE("entry error\n");
		return ;
	}

	be = entry / 8;
	rem = entry % 8;
	header->dirtyBit[be] &= (~(1 << rem)) & 0xff ;
}
int serializeCLH(clHeader *header, buffer *buff)
{
	int i;

	if(!header || !header->dirtyBit ||
		!header->posAt|| !buff) 
		return -1;

	if(writeInt(header->cfCount, buff) < 0) 
		return -1;
	if(writeBytes(header->dirtyBit, header->byteCount, 
			buff) < 0)
		return -1;

	for(i = 0; i < header->cfCount; i++){
		if(writeInt(header->posAt[i], buff) < 0)
			return -1;
	}

	return 0;
}

//-1 clean, 不需要replay, else need replay the log 
int getFirstDirtyPos(clHeader *header)
{
	int i, flag = 0;
	int tmp = 0;

	if(!header || !header->dirtyBit || !header->posAt)
		return -1;
	for(i = 0; i < header->cfCount; i++){
		int dirty = 0;

		dirty = isDirty(header, i);
		if(dirty == 1){
			if(flag == 1)
				tmp = (tmp <  header->posAt[i] ? tmp : 
					header->posAt[i]);
			else{
				flag = 1;
				tmp = header->posAt[i];
			}
		}
	}

	if(flag == 1){
		return tmp;
	}else{
		return -1;
	}
}
clHeader* deserializeCLH(buffer *buff)
{
	clHeader *cur = NULL;
	int cfCount = 0, i;

	if(!buff) 
		return NULL; 

	reset(buff);
	if(readInt(buff, &cfCount) < 0){
		return NULL;
	}
	
	if(!(cur = (clHeader *)calloc(sizeof(clHeader), 1)))
		return NULL;
	if(readBytes(buff, &cur->dirtyBit, &cur->byteCount) < 0){
		freeHeapCLHeader(cur);
		return NULL;
	}
	if(!(cur->posAt = (int *)malloc(sizeof(int) 
					* cfCount))){
		freeHeapCLHeader(cur);
		return NULL;
	}
	
	for(i = 0; i < cfCount; i++){
		if(readInt(buff, &cur->posAt[i]) < 0){
			freeHeapCLHeader(cur);
			return NULL;
		}
	}
	cur->cfCount = cfCount;

	return cur;
}


LIST_HEAD(clList, commitLog) preCLS = \
		LIST_HEAD_INITIALIZER(preCLS);
commitLog *curCL;
/*
 * 下面主要是为commitLog做的一些函数
 */
char *getCommitLogFilePath()
{
	const char *p = NULL;
	char tmp[200] = {0};
	time_t cur;

	if(getDataPath(&p) < 0) 
		return NULL;
	cur = getCurTime();
	sprintf(tmp, "%s/commitlog/commitlog-%lu.db", p, cur);

	return strdup(tmp);
}

commitLog *getCommitLog(char *cp, char type)
{
	commitLog *com = NULL;
	fileAbstract *fa = NULL;
	clHeader *clh = NULL;
	buffer *buff = NULL;
	int cfCount;

	if(type != READ_COMMITLOG && type != 
					WRITE_COMMITLOG)
		return NULL;

	if(getCFCount(&cfCount) < 0) 
		return NULL;

	if(!(com = (commitLog*)calloc(sizeof(commitLog), 1))){
		return NULL;
	}

	if(type == WRITE_COMMITLOG && 
			openWriteFile(cp, COMMITLOGBUFFERSZ, &fa) < 0){
		freeHeapCommitLog(com);
		return NULL;
	}else if(type == READ_COMMITLOG && 
			openReadFile(cp, COMMITLOGBUFFERSZ, &fa) < 0){
		freeHeapCommitLog(com);
		return NULL;
	}	

	if(type == WRITE_COMMITLOG && !(clh = getCLHeader(cfCount))){
		freeHeapCommitLog(com);
		freeHeapFile(fa);
		return NULL;
	}else if(type == READ_COMMITLOG && (
				!(buff = getBuffer()) ||
				readFile(fa, buff) < 0 ||
				!(clh = deserializeCLH(buff)))){
		freeHeapCommitLog(com);
		freeHeapFile(fa);
		if(buff) 
			freeHeapBuffer(buff);
		return NULL;
	}

	if(buff) 
		freeHeapBuffer(buff);

	com->fa = fa;
	com->clh = clh;

	//这里应该确保写入头部正确,如果不能写入头部，表明程序
	//确实有很严重的错误，应该退出
	if(type == WRITE_COMMITLOG && 
			writeHeaderSyn(com , 0) < 0){
		freeHeapCommitLog(com);
		LOG_WRITE("cannot write header\n");
		exit(0);
		return NULL;
	}

	return com;	
}

commitLog *getCommitLogWriter()
{
	char *cp = NULL;
	commitLog *cl = NULL;

	if(!(cp = getCommitLogFilePath())){
   	   return NULL;
	}
	if(!(cl = getCommitLog(cp, WRITE_COMMITLOG))){
		free(cp);
		return NULL;
	}
	free(cp);

	return cl;
}
commitLog *getCommitLogReader(char *fullLogPath)
{
	return getCommitLog(fullLogPath, READ_COMMITLOG);
}
void freeHeapCommitLog(commitLog* cl)
{
	if(!cl) 
		return ;
	freeHeapFile(cl->fa);
	freeHeapCLHeader(cl->clh);
	free(cl);
}

/*获取cl的位置，如果出错，返回-1*/
long getCLPos(commitLog *cl)
{
	if(!cl || !cl->fa){
		LOG_WRITE("arg error\n");
		return -1;
	}

	return getCurFilePos(cl->fa);
}
static int beforeWrite()
{
	long curPos= 0;

	//第一次写commitLog，要求获取一个
	if(!curCL){
		if(!(curCL = getCommitLogWriter())) 
			return -1;
		return 0;
	}

	if((curPos = getCLPos(curCL)) < 0){
		LOG_WRITE("cannot get curpos\n");
		return -1;
	}

	//当commitLog的长度超过我们要求的长度时，
	//在下一个commitLog中写
	if(curPos > COMMITLOGMAXSZ){
		LIST_INSERT_HEAD(&preCLS, curCL, link);
		curCL = NULL;
		return beforeWrite();
	}	
	return 0;
}
//这个函数执行错误了实在是伤不起,直接将进程挂掉
//这里的设计问题，第一次写commitLog的时候，不需要来回移动文件指针
static int writeHeaderSyn(commitLog *cl, int needSeekFile)
{
	long  pos = 0;
	buffer *buff = NULL;

	if(!(buff = getBuffer())){
		return -1;
	}
	if(needSeekFile == 1  && 
			(pos = getCLPos(cl)) < 0){
		freeHeapBuffer(buff);
		return -1;
	}
	
	if(serializeCLH(cl->clh, buff) < 0){
		freeHeapBuffer(buff);
		return -1;
	}
	if(fseekToPos(cl->fa, 0) < 0){
		freeHeapBuffer(buff);
		return -1;
	}
	if(writeAppendFile(cl->fa, buff) < 0){
		freeHeapBuffer(buff);
		//这里必须保证执行正确，不然就会出现很严重的错误
		if(needSeekFile == 1 && 
				fseekToPos(cl->fa, pos) < 0)
			exit(0);
		return -1;
	}
	//写入头部成功，这里就直接使用
	freeHeapBuffer(buff);
	if(needSeekFile == 1 && 
			fseekToPos(cl->fa, pos) < 0)
		exit(0);

	//同步文件
	synFile(cl->fa);
	return 0;
}

int writeCommitLog(buffer *rm, const columnFamilyMetadata *cfmd)
{	
	int cfid;
	unsigned int crc32;
	int ret = 0;

	if(!rm || !cfmd) 
		return -1;
	
	if(getCRC32(rm, &crc32) < 0) 
		return -1;
	cfid = cfmd->cfID;

	if(beforeWrite() < 0)
		return -1;
	ret = isDirty(curCL->clh, cfid);
	if(ret < 0)
		return -1;
	if(ret == 0){
		setDirty(curCL->clh, cfid); 
		writeHeaderSyn(curCL, 1);
	}

	if(writeUnsignedInt(crc32, rm) < 0)
		return -1;
	//这里写失败也会很麻烦
	if(writeAppendFile(curCL->fa, rm) < 0)
		return -1;
	return 0;
}

static int needReplay(rowMutation *rm, commitLog *cl, long pos)
{
	int cfid = 0;
	if(!rm || !cl)
		return -1;
	if(getRMCFID(rm, &cfid) < 0)
		return -1;
	if(isDirty(cl->clh, cfid) == 1 &&
			cl->clh != NULL &&
			cl->clh->posAt[cfid] <= pos)
		return 1;

	return 0;

}
//首先反序列化一个rm，查看该rm所对应的header是否脏
//以及pos是否小于等于rm，如果是，则插入rm
static int recoverFromCurPos(commitLog *reader)
{
	buffer *buff = NULL;
	rowMutation *rm = NULL;
	int ret = -1;

	if(!reader->fa || !reader->clh ||
		!(buff = getBuffer())){
		if(buff)
			freeHeapBuffer(buff);
		return ret;
	}
	while(1){
		unsigned int crc32;
		unsigned int crc;
		long int curPos;

		if(!(rm = (rowMutation *)calloc(sizeof(rowMutation), 1))){
			freeHeapBuffer(buff);
			return ret;
		}
		if((curPos = getCurFilePos(reader->fa)) < 0 ||
				reset(buff) < 0 ||
				readFile(reader->fa, buff) < 0 || 
				reset(buff) < 0 || 
				deserializeRM(buff,rm) < 0 ||
				getCRC32(buff, &crc) < 0 ||
				readUnsignedInt(buff, &crc32) < 0){
			if(endOfFile(reader->fa) == 1)
				ret = 0;	

			freeHeapBuffer(buff);
			freeHeapRM(rm);
			return ret;
		}
		//为什么设计这么复杂的逻辑
		if(crc != crc32 || (needReplay(rm, reader, curPos)  == 1
					&& insertDM(rm) < 0)){
			freeHeapRM(rm);
			continue;
		}
		freeHeapRM(rm);
	}
	//不可能到达这里
	return 0;
}
int auxilary_recover(char *fullLogPath)
{
	commitLog *reader = NULL;
	int firstDirtyPos = 0;

	if(!(reader = getCommitLogReader(fullLogPath))){
		return -1;
	}
	firstDirtyPos = getFirstDirtyPos(reader->clh);
	//我们需要commitLog恢复数据
	if(firstDirtyPos < 0)
		return 0;
	if(fseekToPos(reader->fa, firstDirtyPos) < 0){
		LOG_WRITE("log file seek pos error\n");
		return -1;
	}

	if(recoverFromCurPos(reader) < 0){
		LOG_WRITE("recover error, file is not untouched\n");
		return -1;
	}
	else{
		LOG_WRITE("recover log %s succeed\n", fullLogPath);
	}

	freeHeapCommitLog(reader);
	return 0;
}
//首先获取commitLog目录下的所有commitLog，然后按照
//文件名字排序，最后一个一个处理commitLog文件
int recoverFromCommitLog()
{
	return 0;	
}
int synCommitLog(columnFamilyMetadata *cfmd)
{
	int cfid = 0;
	off_t offset = 0;

	if(!cfmd)
		return -1;

	cfid = cfmd->cfID;

	if(!curCL->clh)
		return -1;
	clearDirty(curCL->clh, cfid);

	if((offset = getCLPos(curCL)) < 0){
		LOG_WRITE("cannot write commitlong\n");
		return -1;
	}

	setEntryPos(curCL->clh, cfid, offset);
	return writeHeaderSyn(curCL, 1);  
}
