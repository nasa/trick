/***************** queue functions ***************/

#ifndef LQUEUE_H
#define LQUEUE_H


#include "trick/dllist.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _LQUEUE
{
    DLLIST list;        /* implementing queue using a linked-list */
}LQUEUE;


LQUEUE* LQ_Create(void);   /* create and initialize a LQUEUE */


void LQ_Init(LQUEUE*);     /* initialize a LQUEUE */


void LQ_EnQ(void* pData,LQUEUE* pQueue); /* insert data into queue */


void* LQ_DeQ(LQUEUE* pQueue);            /* extract data from queue */


void* LQ_Peek(LQUEUE* pQueue);           /* get data off queue without extracting */


int LQ_GetCount(LQUEUE* pQueue);         /* returns number of items in the queue */


#ifdef __cplusplus
}
#endif



#endif


