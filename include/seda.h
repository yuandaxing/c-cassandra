#ifndef _SEDA_H
#define _SEDA_H

#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>
#include "tree.h"
#include "queue.h"
#include "thread.h"
#include "handler.h"
#include "stage.h"
//KISS 

typedef RB_HEAD(stage_head, stage) stage_set;
typedef struct stage_manager{
	stage_set stages;
	pthread_mutex_t lock;
}stage_manager;

int register_stage(char *stageName, int nThread);
int deallocStage(char *name);
int dispatchStage(char *name, void (*handler)(void *), void *arg);

#ifdef __cplusplus /* we could use seda in c++*/
}
#endif
#endif
