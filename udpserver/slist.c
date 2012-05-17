/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-07-25 16:18
*
* Filename:slist.c
*
* Description: 
*
=============================================================================*/
#include "slist.h"


void initialize_queue(queue_t *q)
{
        q->head=q->tail=NULL;
#ifdef  MUTEX
	pthread_mutex_init(&(q->mutex), NULL);
#endif
}


node_t * get_node()
{
        node_t *n = (node_t*) malloc(sizeof(node_t));
	
        if(!n){
                printf("out of memory\n");
                exit(0);
        }
	//n->size=0;
        n->next=NULL;
        return n;
}



void put_node(node_t *p)
{	
//	printf("address:%0x size:%d\n", p, p->size);
        free(p);
}



static int empty(queue_t *q)
{
        return ( q->head==NULL && q->tail==NULL); 
}


static int only_one_node(queue_t *q)
{
        return q->head!=NULL && q->head==q->tail;
}

node_t *  push_into_queue(node_t * n, queue_t *q)
{
#ifdef  MUTEX
	pthread_mutex_lock(&(q->mutex));
#endif

        if(empty(q)){
                q->tail=q->head=n;
        }
        else{
                q->tail->next=n;
                q->tail=n;
                n->next=0;
        }

#ifdef  MUTEX
	pthread_mutex_unlock(&(q->mutex));
#endif
        return n;
}


node_t * pop_from_queue(queue_t *q)
{
        node_t *n;

#ifdef  MUTEX
	pthread_mutex_lock(&(q->mutex));
#endif
        if(empty(q)){
#ifdef  MUTEX
		pthread_mutex_unlock(&(q->mutex));
#endif
                return NULL;
        }


        if(only_one_node(q)){
                n=q->head;
                q->head=q->tail=NULL;
#ifdef  MUTEX
		pthread_mutex_unlock(&(q->mutex));
#endif
                return n;
        }
        n=q->head;
        q->head=n->next;
#ifdef  MUTEX
	pthread_mutex_unlock(&(q->mutex));
#endif
        return n;
}

