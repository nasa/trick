
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

void DLL_Delete(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "List is NULL");
        return;
    }

    if (list) {
        DLL_RemoveAll(list);
        free(list);
    }
}


/* returns number of elements in list */
/* return: count */

void DLL_Init(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "List is NULL");
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
        fprintf(stderr, "List is NULL");
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
        fprintf(stderr, "List is NULL");
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
        fprintf(stderr, "List is NULL");
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
        fprintf(stderr, "List is NULL");
        return NULL;
    }


    if (n < 0 && n >= list->count) {
        fprintf(stderr, "Index is either negative or exceeds the number of elements in the list");
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

    if (list == NULL) {
        fprintf(stderr, "List is NULL");
        return NULL;
    }


    if (pos == NULL) {
        fprintf(stderr, "Position is NULL");
        return NULL;
    }

    if (list) {
        return pos->info;
    }
    return (NULL);
}


/* sets info at a position */
/* return: the info previously at pos */

void *DLL_SetAt(DLLPOS pos, void *info, DLLIST * list)
{

    void *ret;


    if (pos == NULL && list == NULL) {
        fprintf(stderr, "List and Position are NULL");
        return NULL;
    }

    if (pos != NULL && list != NULL) {
        ret = pos->info;
        pos->info = info;
        return ret;
    }
    return (NULL);
}


/* removes a position from the list */
/* return: the info at the position being removed */

void *DLL_RemoveAt(DLLPOS pos, DLLIST * list)
{
    void *ret;


    if (pos == NULL && list == NULL) {
        fprintf(stderr, "List and Position are NULL");
        return NULL;
    }

    if (pos != NULL && list != NULL) {
        list->count--;
        if (pos->prev != NULL) {
            pos->prev->next = pos->next;
            if (pos->next != NULL)
                pos->next->prev = pos->prev;
            else
                list->tail = pos->prev;
        } else if (pos->next != NULL) {
            list->head = pos->next;
            pos->next->prev = NULL;
        } else {
            list->head = NULL;
            list->tail = NULL;
        }
        ret = pos->info;

        free(pos);
        return ret;

    }
    return (NULL);
}


/* removes all elements in the list */
/* return: none */

void DLL_RemoveAll(DLLIST * list)
{
    DLLPOS pos = DLL_GetHeadPosition(list);
    DLLPOS prev = NULL;


    if (list == NULL) {
        fprintf(stderr, "List is NULL");
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


        if (pos == NULL && list == NULL) {
            fprintf(stderr, "List and Position are NULL");
            return NULL;
        }

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
        return newpos;
    }
    return (NULL);
}


/* inserts an element after pos */
/* return: the new pos holding data */

DLLPOS DLL_InsertAfter(DLLPOS pos, void *data, DLLIST * list)
{
    if (pos != NULL && list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));


        if (pos == NULL && list == NULL) {
            fprintf(stderr, "List and Position are NULL");
            return NULL;
        }

        newpos->info = data;
        if (pos->next == NULL) {
            newpos->next = list->tail;
            newpos->next = NULL;
            list->tail = newpos;
            pos->next = newpos;
        } else {
            pos->next->prev = newpos;
            newpos->next = pos->next;
            newpos->prev = pos;
            pos->next = newpos;
        }
        return newpos;
    }

    return (NULL);
}


/* gets the next position in the list */
/* return: the data at the current pos */

void *DLL_GetNext(DLLPOS * pos, DLLIST * list)
{
    void *ret;


    if (pos == NULL && list == NULL) {
        fprintf(stderr, "List and Position are NULL");
        return NULL;
    }

    if (pos != NULL && list != NULL) {
        ret = (*pos)->info;
        *pos = (*pos)->next;
        return ret;
    }
    return (NULL);
}


/* gets the previous position in the list */
/* return: the data at the current position */

void *DLL_GetPrev(DLLPOS * pos, DLLIST * list)
{
    void *ret;


    if (pos == NULL && list == NULL) {
        fprintf(stderr, "List and Position are NULL");
        return NULL;
    }

    if (pos != NULL && list != NULL) {
        ret = (*pos)->info;
        *pos = (*pos)->prev;
        return ret;
    }
    return (NULL);
}


/* adds an element to the head of the list */
/* return: the position of the new element */

DLLPOS DLL_AddHead(void *data, DLLIST * list)
{
    if (list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));


        if (list == NULL) {
            fprintf(stderr, "List is NULL");
            return NULL;
        }

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
    return (NULL);
}


/* adds an element to the tail of the list */
/* return: the position of the new element */

DLLPOS DLL_AddTail(void *data, DLLIST * list)
{
    if (list != NULL) {
        DLLPOS newpos = (DLLPOS) malloc(sizeof(DLLNODE));


        if (list == NULL) {
            fprintf(stderr, "List is NULL");
            return NULL;
        }

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
    return (NULL);
}


/* gets the position of the head element */
/* return: the position of the head element */

DLLPOS DLL_GetHeadPosition(DLLIST * list)
{

    if (list == NULL) {
        fprintf(stderr, "List is NULL");
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
        fprintf(stderr, "List is NULL");
        return NULL;
    }

    if (list) {
        return list->tail;
    }
    return (NULL);
}
