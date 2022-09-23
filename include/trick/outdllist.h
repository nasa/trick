
#ifndef OUTDLLIST_H
#define OUTDLLIST_H

#include <assert.h>


#ifdef _DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DLLNODE {
    void* info;
    struct _DLLNODE* prev;
    struct _DLLNODE* next;
}DLLNODE;

typedef DLLNODE* DLLPOS;

typedef struct _DLLIST {
    int count;
    DLLNODE* head;
    DLLNODE* tail;
    int (*compare)(void* info1,void* info2);
}DLLIST;


DLLIST* DLL_Create(void);

void DLL_Delete(DLLIST*);

void DLL_Init(DLLIST*);

int DLL_GetCount(DLLIST*);

void* DLL_Find(void* data,DLLIST*);

DLLPOS DLL_FindIndex(int n,DLLIST*);

void* DLL_GetAt(DLLPOS pos,DLLIST*);

void* DLL_SetAt(DLLPOS pos,void* data,DLLIST*);

void* DLL_RemoveAt(DLLPOS,DLLIST*);

void DLL_RemoveAll(DLLIST*);

DLLPOS DLL_InsertBefore(DLLPOS pos,void* data,DLLIST*);

DLLPOS DLL_InsertAfter(DLLPOS pos,void* data,DLLIST*);

void* DLL_GetNext(DLLPOS*,DLLIST*);

void* DLL_GetPrev(DLLPOS*,DLLIST*);

DLLPOS DLL_AddHead(void*, DLLIST*);

DLLPOS DLL_AddTail(void*, DLLIST*);

DLLPOS DLL_GetHeadPosition(DLLIST*);

DLLPOS DLL_GetTailPosition(DLLIST*);

#ifdef __cplusplus
extern "C" {
#endif


#endif

