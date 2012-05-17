#ifndef _SLIST_H_
#define _SLIST_H_
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <event.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct __slist{
        struct __slist *next;
		struct event e;
}node_t;

typedef struct __queue{
        node_t *head;
        node_t *tail;
#ifdef  MUTEX
	pthread_mutex_t mutex;
#endif
}queue_t;



node_t * get_node();
void initialize_queue(queue_t *q);
void put_node(node_t *p);
node_t *  push_into_queue(node_t * n, queue_t *q);
node_t * pop_from_queue(queue_t *q);

#ifdef __cplusplus
}
#endif
//typedef st  __DATA_TYPE[10][150];


#endif
