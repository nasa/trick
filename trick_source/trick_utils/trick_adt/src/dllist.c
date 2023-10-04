
#include <stdlib.h>
#include <stdio.h>
#include "trick/dllist.h"

/* create and initialize a DLLIST */
/* return: pointer to the new list */

DLLIST *DLL_Create(void)
{
    DLLIST *ret = (DLLIST *) malloc(sizeof(DLLIST));
    ret->count = 0;
    ret->head = NULL;
    ret->tail = NULL;
    ret->compare = NULL;
    return ret;
}


/* initializes a list */
/* return: none */
// JMP: This should  return a status to let us know whether or not we succeeded
void DLL_Delete(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return;
    }

    if (list) {
        DLL_RemoveAll(list);
        free(list);
    }
}

/* Determine whether the given list contains the node at the given pos.
   If it does, return 1, otherwise return false.
*/
static int DLL_ListContainsPos( DLLPOS pos, DLLIST * list) {
     
    if ((list == NULL) || (pos == NULL)) {
        return 0;
    }
    DLLPOS lpos = list->head;
    while ((lpos != pos) && (lpos != NULL)) {
        lpos = lpos->next;
    }
    if (lpos == NULL) {
        return 0;
    }
    return 1;
}

// JMP: This should return a status to let us know whether or not we succeeded
void DLL_Init(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return;
    }

    if (list) {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
        list->compare = NULL;
    }
}


/* returns number of elements in list */
/* return: count */

int DLL_GetCount(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return -1;
    }

    if (list) {
        return list->count;
    }
    return (0);
}


/* find an element matching data */
/* return: pointer to found element or NULL */

void *DLL_Find(void *data, DLLIST * list)
{
    DLLPOS pos;

    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    if (list->compare == NULL) {
        fprintf(stderr, "Error (%s): List doesn't have a compare function.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    pos = DLL_GetHeadPosition(list);
    while (pos != NULL) {
        DLLPOS prevpos = pos;
        if (list->compare(DLL_GetNext(&pos, list), data))
            return DLL_GetAt(prevpos, list);
    }
    return NULL;
}


/* find position that holds the same address as data */
/* return: position of a matching element */

DLLPOS DLL_FindPos(void *data, DLLIST * list)
{
    DLLPOS pos;


    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    pos = DLL_GetHeadPosition(list);
    while (pos != NULL) {
        if (DLL_GetAt(pos, list) == data)
            return pos;
        DLL_GetNext(&pos, list);
    }
    return NULL;
}


/* find position at given index */
/* return: position of nth element */


DLLPOS DLL_FindIndex(int n, DLLIST * list)
{
    int nPos = 0;
    DLLPOS pos;


    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    if ( (n < 0) || (n >= list->count) ) {
        fprintf(stderr, "Error (%s): Index is either negative or exceeds the number of elements in the list.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    pos = DLL_GetHeadPosition(list);
    while (pos != NULL) {
        if (nPos++ == n)
            break;
        DLL_GetNext(&pos, list);
    }
    return pos;
}


/* gets info at a position */
/* return: the info at pos */

void *DLL_GetAt(DLLPOS pos, DLLIST * list)
{
    if ((list == NULL) || (pos == NULL)) {
        fprintf(stderr, "Error (%s): Either pos, list, or both are NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    if ( !DLL_ListContainsPos(pos, list) ) {
        fprintf(stderr, "Error (%s): The specified list doesn't contain the specified node (pos).\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    return pos->info;
}


/* sets info at a position */
/* return: the info previously at pos */

void *DLL_SetAt(DLLPOS pos, void *info, DLLIST * list)
{
    if ((list == NULL) || (pos == NULL)) {
        fprintf(stderr, "Error (%s): Either pos, list, or both are NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    if ( !DLL_ListContainsPos(pos, list) ) {
        fprintf(stderr, "Error (%s): The specified list doesn't contain the specified node (pos).\n", __FUNCTION__);
        fflush(stderr); 
        return NULL;
    }
    void* prev_info = pos->info;
    pos->info = info;
    return prev_info;
}


/* removes a position from the list */
/* return: the info at the position being removed */

void *DLL_RemoveAt(DLLPOS pos, DLLIST * list)
{
    void *ret;

    if ((pos == NULL) || (list == NULL)) {
        fprintf(stderr, "Error (%s): List or Position is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    if ( !DLL_ListContainsPos(pos, list) ) {
        fprintf(stderr, "Error (%s): The specified list doesn't contain the specified node (pos).\n", __FUNCTION__);
        fflush(stderr); 
        return NULL;
    }

    if (pos->prev != NULL) {             // check whether pos is the head
        pos->prev->next = pos->next;     // pos is not the head
        if (pos->next != NULL)           // check whether pos is the tail
            pos->next->prev = pos->prev; // pos is not the tail
        else
            list->tail = pos->prev;      // pos is the tail
    } else if (pos->next != NULL) {      // pos is the head, check whether it's also the tail
        list->head = pos->next;          // pos not the tail
        pos->next->prev = NULL;
    } else {                             // pos is both the head and the tail
        list->head = NULL;
        list->tail = NULL;
    }

    list->count --;
    ret = pos->info;
    free(pos);
    return ret;

}


/* removes all elements in the list */
/* return: none */

void DLL_RemoveAll(DLLIST * list)
{
    DLLPOS pos = DLL_GetHeadPosition(list);
    DLLPOS prev = NULL;


    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return;
    }

    while (pos != NULL) {
        prev = pos;
        DLL_GetNext(&pos, list);
        DLL_RemoveAt(prev, list);
    }
}


/* inserts an element before pos */
/* return: the new pos holding data */

DLLPOS DLL_InsertBefore(DLLPOS pos, void *data, DLLIST * list)
{
    if (pos != NULL && list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));
        newpos->info = data;
        if (pos->prev == NULL) {
            newpos->next = list->head;
            newpos->prev = NULL;
            list->head = newpos;
            pos->prev = newpos;
        } else {
            pos->prev->next = newpos;
            newpos->prev = pos->prev;
            newpos->next = pos;
            pos->prev = newpos;
        }
        list->count++;
        return newpos;
    }
    fprintf(stderr, "Error (%s): Either List, Position, or both are NULL.\n", __FUNCTION__);
    fflush(stderr);
    return (NULL);
}


/* inserts an element after pos */
/* return: the new pos holding data */

DLLPOS DLL_InsertAfter(DLLPOS pos, void *data, DLLIST * list)
{
    if (pos != NULL && list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));
        newpos->info = data;
        if (pos->next == NULL) {
            newpos->prev = list->tail;
            newpos->next = NULL;
            list->tail = newpos;
            pos->next = newpos;
        } else {
            pos->next->prev = newpos;
            newpos->next = pos->next;
            newpos->prev = pos;
            pos->next = newpos;
        }
        list->count++;
        return newpos;
    }
    fprintf(stderr, "Error (%s): Either List, Position, or both are NULL.\n", __FUNCTION__);
    fflush(stderr);
    return (NULL);
}

/* gets the next position in the list */
/* return: the data at the current pos */

void *DLL_GetNext(DLLPOS* pos_p, DLLIST * list)
{
    if ((list == NULL) || (pos_p == NULL)) {
        fprintf(stderr, "Error (%s): Either the List, the Position pointer, or both are NULL.\n", __FUNCTION__);
        fflush(stderr);
        return (NULL);
    }
    if ( !DLL_ListContainsPos(*pos_p, list) ) {
        fprintf(stderr, "Error (%s): List doesn't contain a node at the given position.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    void* data = (*pos_p)->info;
    *pos_p = (*pos_p)->next;
    return data;
}

/* gets the previous position in the list */
/* return: the data at the current position */

void *DLL_GetPrev(DLLPOS * pos_p, DLLIST * list)
{
    if ((list == NULL) || (pos_p == NULL)) {
        fprintf(stderr, "Error (%s): Either the List, the Position pointer, or both are NULL.\n", __FUNCTION__);
        fflush(stderr);
        return (NULL);
    }
    if ( !DLL_ListContainsPos(*pos_p, list) ) {
        fprintf(stderr, "Error (%s): List doesn't contain a node at the given position.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }
    void* data = (*pos_p)->info;
    *pos_p = (*pos_p)->prev;
    return data;
}

/* adds an element to the head of the list */
/* return: the position of the new element */

DLLPOS DLL_AddHead(void *data, DLLIST * list)
{
    if (list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));
        newpos->info = data;
        if (list->head == NULL) {
            list->head = newpos;
            list->tail = newpos;
            newpos->next = NULL;
            newpos->prev = NULL;
        } else {
            newpos->next = list->head;
            list->head = newpos;
            newpos->prev = NULL;
            newpos->next->prev = newpos;
        }
        list->count++;
        return newpos;
    }
    fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
    fflush(stderr);
    return (NULL);
}


/* adds an element to the tail of the list */
/* return: the position of the new element */

DLLPOS DLL_AddTail(void *data, DLLIST * list)
{
    if (list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));
        newpos->info = data;
        if (list->tail == NULL) {
            list->head = newpos;
            list->tail = newpos;
            newpos->next = NULL;
            newpos->prev = NULL;
        } else {
            newpos->next = NULL;
            newpos->prev = list->tail;
            newpos->prev->next = newpos;
            list->tail = newpos;
        }
        list->count++;
        return newpos;
    }
    fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
    fflush(stderr);
    return (NULL);
}


/* gets the position of the head element */
/* return: the position of the head element */

DLLPOS DLL_GetHeadPosition(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    if (list) {
        return list->head;
    }
    return (NULL);
}


/* gets the position of the tail element */
/* return: the position of the tail element */

DLLPOS DLL_GetTailPosition(DLLIST * list)
{
    if (list == NULL) {
        fprintf(stderr, "Error (%s): List is NULL.\n", __FUNCTION__);
        fflush(stderr);
        return NULL;
    }

    if (list) {
        return list->tail;
    }
    return (NULL);
}
