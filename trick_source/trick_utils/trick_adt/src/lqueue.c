
/*************** implementation of the queue ***************/

#include "trick/lqueue.h"
#include "trick/dllist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* create and initialize an LQUEUE structure */

LQUEUE * LQ_Create(void)
{
    LQUEUE * ret = (LQUEUE *)malloc(sizeof(LQUEUE));
    DLL_Init(&(ret->list));
    return ret;
}

int LQ_Delete(LQUEUE * lqueue)
{
    if(lqueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return -1;
    }
    DLL_Delete(&lqueue->list);
    return 0;
}

/* initialize a queue created statically */
void LQ_Init(LQUEUE * pQueue)
{
    if(pQueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.\n", __FUNCTION__);
        fflush(stderr);
    }
    DLL_Init(&(pQueue->list));
}

/* insert an element into the queue */

void LQ_EnQ(void * pData, LQUEUE * pQueue)
{
    if(pQueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.", __FUNCTION__);
        return;
    }

    DLL_AddHead(pData, &(pQueue->list));
}

/* remove an element from the queue */

void * LQ_DeQ(LQUEUE * pQueue)
{
    if(pQueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.", __FUNCTION__);
        return NULL;
    }

    if(LQ_GetCount(pQueue) > 0)
    {
        return DLL_RemoveAt(DLL_GetTailPosition(&(pQueue->list)), &(pQueue->list));
    }
    return NULL;
}

/* return the element the is at the tail of the queue without removing if from the queue */

void * LQ_Peek(LQUEUE * pQueue)
{
    if(pQueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    if(DLL_GetCount(&(pQueue->list)) <= 0)
    {
        fprintf(stderr, "Error (%s): LQUEUE is empty.", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    return DLL_GetAt(DLL_GetTailPosition(&(pQueue->list)), &(pQueue->list));
}

/* returns the number of elements in the queue */

int LQ_GetCount(LQUEUE * pQueue)
{
    if(pQueue == NULL)
    {
        fprintf(stderr, "Error (%s): Pointer to LQUEUE is NULL.", __FUNCTION__);
        return -1;
    }

    return DLL_GetCount(&(pQueue->list));
}
