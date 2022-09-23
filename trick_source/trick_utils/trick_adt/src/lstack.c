/******** implementation of LSTACK *************/

#include <stdlib.h>
#include "trick/lstack.h"
#include "trick/dllist.h"


/* create an initialize a LSTACK structure */

LSTACK *LS_Create(void)
{
    LSTACK *ret;
    ret = (LSTACK *) malloc(sizeof(LSTACK));
    DLL_Init(&(ret->list));
    return ret;
}


/* initialize a statically allocated LSTACK */

void LS_Init(LSTACK * pStack)
{
    DLL_Init(&(pStack->list));
}


/* remove an element from the top of the stack */

void *LS_Pop(LSTACK * pStack)
{
    return DLL_RemoveAt(DLL_GetHeadPosition(&(pStack->list)), &(pStack->list));
}


/* get the element on the top of the stack without removing it from the stack */
void *LS_Peek(LSTACK * pStack)
{
    return DLL_GetAt(DLL_GetHeadPosition(&(pStack->list)), &(pStack->list));
}


/* insert an element on the top of the stack */

void LS_Push(void *pData, LSTACK * pStack)
{
    DLL_AddHead(pData, &(pStack->list));
}


/* return the number of elements on the stack */

int LS_GetCount(LSTACK * pStack)
{
    return DLL_GetCount(&(pStack->list));
}
