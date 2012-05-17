/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-07-25 16:18
*
* Filename:log.c
*
* Description: 
*
=============================================================================*/
#include "log.h"

//static int init = 0;
static FILE *fp = NULL;
char *name = NULL;

static int open_file(char *na);


int set_log(char *n)
{
	int ret = 0;
	if(n== NULL || strlen(n) == 0)
		return -1;

	name = (char *)malloc(strlen(n)+1);
	if(name == NULL){
		printf("we have not enough memory\n");
		return -1;
	}
	
	strcpy(name, n);
	
	ret = open_file(name);

	if(ret < 0){
		free(name);
		fp = NULL;
		name = NULL;
		return -1;
	}
	
	return 0;
	
}
	
static int close_file()
{
	if(fp)
		return fclose(fp);
	
	return -1;
}

static int open_file(char *na)
{
	fp = fopen(na, "a");
	
	if(fp == NULL){
		fprintf(stderr, "open log failed\n");
		return -1;
	}
	
	return 0;
}
/*static int reopen_file()
{
	close_file();
	return open_file(name);
}*/
static int write_file(char *str)
{
	if(fp != NULL){
		fprintf(fp, "%s", str); 
		fflush(fp);
		return 0;
	}

	else
		return fprintf(stderr, "%s", str);
	return 0;

}

int close_log()
{
	return close_file();
}
int _log_write(char *file, int line, const char *fmt,...)
{
	int ret = 0;
	char buffer[1000];
	va_list ap;
	time_t cur_time;
	struct tm tm_now;
	char timestamp[20];


/*make sure open the file*/

/*the log formate should like "yy-mm-dd hh-mm-ss" */
	memset(buffer, 0, sizeof(buffer));

	memset(timestamp, 0, sizeof(timestamp));
	time(&cur_time);
	localtime_r(&cur_time, &tm_now);
	strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", &tm_now);
	sprintf(buffer, "%s: ", timestamp);
	ret = strlen(buffer);
	sprintf(buffer + ret, "%s:%d ", file, line);

	ret = strlen(buffer);
	va_start(ap, fmt);
	vsprintf(buffer + ret, fmt, ap);
	va_end(ap);

	return write_file(buffer);
}
