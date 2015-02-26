
/*
  PURPOSE: (link list)
  ICG:     (No)
*/

/*********** doubly linked list types *************/

/*
 * $Log: dllist.h,v $
 * Revision 7.3  2008-05-20 11:33:41-05  lin
 * Remove warnings, part 3
 *
 * Revision 7.2  2007-12-04 17:11:27-06  penn
 * Deal with the dire, inquitous, and even nefarious warnings from a whole heap of -W flags
 *
 * Revision 7.1  2006-06-22 14:09:06-05  lin
 * Bump version number for 07
 *
 * Revision 5.3  2005-03-17 11:22:09-06  lin
 * Finish 8 Space Indent
 *
 * Revision 5.2  2004-11-03 15:00:12-06  lin
 * Investigate using aio_write
 *
 * Revision 4.2  2004/01/16 21:32:18  lin
 * Add a log to all files
 *
 */

#ifndef _DLLIST_H
#define _DLLIST_H

#include <assert.h>


#ifdef _DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#ifdef SWIG
#define SWIGTYPE_p_DLLIST SWIGTYPE_p__DLLIST
#define SWIGTYPE_p_DLLNODE SWIGTYPE_p__DLLNODE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***** data structures ***********/


typedef struct _DLLNODE
{
    void* info;
    struct _DLLNODE* prev;
    struct _DLLNODE* next;
}DLLNODE;

typedef DLLNODE* DLLPOS;


typedef struct _DLLIST
{
    int count;
    DLLNODE* head;
    DLLNODE* tail;
    int (*compare)(void* info1,void* info2);
}DLLIST;
      


/******************** function prototypes ********************/

DLLIST* DLL_Create(void);     /* create and initialize a DLLIST */
                              /* return: pointer to the new list */

void DLL_Delete(DLLIST*);     /* delete a DLLIST (does not delete info) */
                              /* return: none */

void DLL_Init(DLLIST*);       /* initializes a list */
                              /* return: none */

int DLL_GetCount(DLLIST*);    /* returns number of elements in list */
                              /* return: count */

void* DLL_Find(void* data,DLLIST*);           /* find an element matching data */
                                              /* return: pointer to found element or NULL */

DLLPOS DLL_FindPos(void* data,DLLIST*);       /* find position that holds the same address as data */
                                              /* return: position of a matching element */

DLLPOS DLL_FindIndex(int n,DLLIST*);          /* find position at given index */
                                              /* return: position of nth element */

void* DLL_GetAt(DLLPOS pos,DLLIST*);          /* gets info at a position */
                                              /* return: the info at pos */ 

void* DLL_SetAt(DLLPOS pos,void* data,DLLIST*);          /* sets info at a position */
                                              /* return: the info previously at pos */ 

void* DLL_RemoveAt(DLLPOS,DLLIST*);           /* removes a position from the list */
                                              /* return: the info at the position being removed */

void DLL_RemoveAll(DLLIST*);                  /* removes all elements in the list */
                                              /* return: none */ 

DLLPOS DLL_InsertBefore(DLLPOS pos,void* data,DLLIST*);  /* inserts an element before pos */
                                              /* return: the new pos holding data */

DLLPOS DLL_InsertAfter(DLLPOS pos,void* data,DLLIST*);   /* inserts an element after pos */    
                                              /* return: the new pos holding data */

void* DLL_GetNext(DLLPOS*,DLLIST*);            /* gets the next position in the list */
                                               /* return: the data at the current pos */

void* DLL_GetPrev(DLLPOS*,DLLIST*);             /* gets the previous position in the list */
                                               /* return: the data at the current position */

DLLPOS DLL_AddHead(void*, DLLIST*);            /* adds an element to the head of the list */
                                               /* return: the position of the new element */

DLLPOS DLL_AddTail(void*, DLLIST*);            /* adds an element to the tail of the list */
                                               /* return: the position of the new element */

DLLPOS DLL_GetHeadPosition(DLLIST*);           /* gets the position of the head element */
                                               /* return: the position of the head element */

DLLPOS DLL_GetTailPosition(DLLIST*);           /* gets the position of the tail element */
                                               /* return: the position of the tail element */

int bubble_sort ( DLLIST * list);

#ifdef __cplusplus
}
#endif


#endif


