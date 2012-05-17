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
#else
#define LOG_WRITE(...) 
#endif

#ifdef __cplusplus
}
#endif
#endif
