
/*************** implementation of the queue ***************/

#include <stdlib.h>
#include <stdio.h>
#include "trick/lqueue.h"
#include "trick/dllist.h"
#include <assert.h>

/* create and initialize an LQUEUE structure */

LQUEUE *LQ_Create(void)
{
    LQUEUE *ret = (LQUEUE *) malloc(sizeof(LQUEUE));
    DLL_Init(&(ret->list));
    return ret;
}


/* initialize a queue created statically */
void LQ_Init(LQUEUE * pQueue)
{

    DLL_Init(&(pQueue->list));
}


/* insert an element into the queue */

void LQ_EnQ(void *pData, LQUEUE * pQueue)
{

    if (pQueue == NULL) {
        fprintf(stderr, "Queue is NULL");
        return;
    }

    DLL_AddHead(pData, &(pQueue->list));
}


/* remove an element from the queue */

void *LQ_DeQ(LQUEUE * pQueue)
{
    if (LQ_GetCount(pQueue) > 0)
        return DLL_RemoveAt(DLL_GetTailPosition(&(pQueue->list)), &(pQueue->list));
    return NULL;
}


/* return the element the is at the tail of the queue without removing if from the queue */

void *LQ_Peek(LQUEUE * pQueue)
{

    if (pQueue == NULL && DLL_GetCount(&(pQueue->list)) <= 0) {
        fprintf(stderr, "Queue is NULL and number of elements is 0");
        return NULL;
    }

    if (pQueue != NULL)
        return DLL_GetAt(DLL_GetTailPosition(&(pQueue->list)), &(pQueue->list));
    return NULL;
}


/* returns the number of elements in the queue */

int LQ_GetCount(LQUEUE * pQueue)
{

    if (pQueue == NULL) {
        fprintf(stderr, "Queue is NULL");
        return -1;
    }

    return DLL_GetCount(&(pQueue->list));
}
