#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef	__cplusplus
extern "C" {
#endif

/* if we used the log file, we should close the log*/
int close_log();
int set_log(char *name);
int _log_write(char *file, int line, const char *fmt,...);
#ifdef DEBUG
#define LOG_WRITE(...) _log_write(__FILE__, __LINE__, __VA_ARGS__)

#define POINTER_NOT_NULL(POINTER)	if(!POINTER){	\
										LOG_WRITE("POINTER %s should not NULL", #POINTER);	\
										return -1; \
									}

#define NOT_LESS_THAN(var, THRESHOLD) if(var < THRESHOLD){	\
										LOG_WRITE("%s should not less than %d\n", #var, THRESHOLD);	\
										return -1; \
									  }

#define FILE_DES_VALID(fil)		if(fil < 3){	\
										LOG_WRITE("file num:%d\n", fil); \
										return -1; \
								}
#else
#define LOG_WRITE(...) 
#define POINTER_NOT_NULL(POINTER)	
#endif

#ifdef __cplusplus
}
#endif
#endif
