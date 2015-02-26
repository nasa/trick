/************** stack functions *****************/

/*
 * $Id: lstack.h 49 2009-02-02 22:37:59Z lin $
 */

#ifndef _LSTACK_H
#define _LSTACK_H


#include "dllist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _LSTACK
{
    DLLIST list;   /* implementing the stack using a list */
}LSTACK;


LSTACK* LS_Create(void);   /* creates and initializes a LSTACK */


void LS_Init(LSTACK*);    /* initializes a LSTACK */


void* LS_Pop(LSTACK*);    /* pop an item off the stack */


void* LS_Peek(LSTACK*);   /* get item at top of stack without poping */


void LS_Push(void* pData,LSTACK*);    /* push an item on the stack */


int LS_GetCount(LSTACK*); /* get number of items on the stack */

#ifdef __cplusplus
}
#endif

#endif


