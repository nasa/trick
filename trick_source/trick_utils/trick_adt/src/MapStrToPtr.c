
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "trick/MapStrToPtr.h"

/* create a new table entry structure */

TBLENTRY *NewTblEntry(char *parmKey, void *parmData)
{
    TBLENTRY *pEntry = (TBLENTRY *) malloc(sizeof(TBLENTRY));   /* allocate memory */
    pEntry->key = strdup(parmKey);     /* initialize elements of the table entry */
    pEntry->data = parmData;
    return pEntry;
}

/* free memory of a table entry */
void DeleteTblEntry(TBLENTRY * entry)
{
    free(entry->key);
    free(entry);
}


/* create an iterator */
StrMapIterator *SMI_Create(MapStrToPtr * pTable)
{
    StrMapIterator *pIter;

    if (pTable == NULL) {
        fprintf(stderr, "Table is NULL");
        return NULL;
    }

    pIter = (StrMapIterator *) malloc(sizeof(StrMapIterator));

    pIter->m_pTable = pTable;
    pIter->m_Index = 0;                /* set the current index of the list array to 0 */
    pIter->m_ListPos = NULL;           /* set the position reference to null indicating there is not a current element */
    return pIter;
}

/* free the iterator memory */
void SMI_Delete(StrMapIterator * pIter)
{
    free(pIter);
}


/* attach the iterator to a different map and re-initialize the iterator */
void SMI_Attach(MapStrToPtr * pTable, StrMapIterator * pIter)
{

    if (pTable == NULL) {
        fprintf(stderr, "Table is NULL");
        return;
    }

    pIter->m_Index = 0;                /* initialize the current index */
    pIter->m_ListPos = NULL;           /* initialize the list position reference to indicate no current element */
    pIter->m_pTable = pTable;          /* save pointer to the map */
}



/* gets the first element in the string map */

void *SMI_GetFirst(char **key, StrMapIterator * pIter)
{
    int i;
    pIter->m_Index = 0;                /* set the list array index to 0 */
    pIter->m_ListPos = NULL;           /* initialize the position to 0 */
    if (pIter->m_pTable->m_Count == 0) /* make sure at least one element is in the string map */
        return NULL;
    for (i = 0; i < pIter->m_pTable->m_Size; i++) {     /* find the first list in the list array that has an element */
        DLLPOS pos = DLL_GetHeadPosition(pIter->m_pTable->m_ppList[i]);
        if (pos != NULL) {
            TBLENTRY *pEntry = (TBLENTRY *) DLL_GetAt(pos, pIter->m_pTable->m_ppList[i]);
            pIter->m_Index = i;        /* set the index to the index of this list */
            pIter->m_ListPos = pos;    /* set the position to the head position of this list */
            *key = pEntry->key;        /* set the key to the key at this position */
            return pEntry->data;       /* return the data at this location */
        }
    }

    return NULL;
}



/* gets the element after the element previously accessed */

void *SMI_GetNext(char **key, StrMapIterator * pIter)
{
    /* make sure there is a previous element */
    if (pIter->m_ListPos == NULL) {
        fprintf(stderr, "There is no previous element");
        return NULL;
    }

    /* get the next element in the current list */
    DLL_GetNext(&(pIter->m_ListPos), pIter->m_pTable->m_ppList[pIter->m_Index]);

    if (pIter->m_ListPos != NULL) {    /* found the next entry in the current list */
        TBLENTRY *pEntry = (TBLENTRY *) DLL_GetAt(pIter->m_ListPos, pIter->m_pTable->m_ppList[pIter->m_Index]);
        *key = pEntry->key;
        return pEntry->data;           /* return the next position in the current list */
    }

    else if (pIter->m_ListPos == NULL) {        /* if we are already at the end of the current list then find the next
                                                   list that is not empty */
        pIter->m_Index++;
        while (pIter->m_Index < pIter->m_pTable->m_Size) {
            pIter->m_ListPos = DLL_GetHeadPosition(pIter->m_pTable->m_ppList[pIter->m_Index]);
            if (pIter->m_ListPos != NULL) {
                TBLENTRY *pEntry = (TBLENTRY *) DLL_GetAt(pIter->m_ListPos, pIter->m_pTable->m_ppList[pIter->m_Index]);
                *key = pEntry->key;
                return pEntry->data;   /* return the head position in the new current list */
            }
            pIter->m_Index++;          /* iterate through the list array */
        }
        pIter->m_Index = 0;
    }
    return NULL;
}


/* gets the element before the previouly accessed element */

void *SMI_GetPrev(char **key, StrMapIterator * pIter)
{
    /* make sure there was a previously accessed element */
    if (pIter->m_ListPos == NULL) {
        fprintf(stderr, "There was no previously accessed element");
        return NULL;
    }

    /* get the previous element of the current list */
    DLL_GetPrev(&(pIter->m_ListPos), pIter->m_pTable->m_ppList[pIter->m_Index]);
    if (pIter->m_ListPos == NULL) {    /* if we are already at the beginning of the list the check the previous lists */
        while (pIter->m_Index > 0) {
            pIter->m_Index--;
            pIter->m_ListPos = DLL_GetTailPosition(pIter->m_pTable->m_ppList[pIter->m_Index]);
            if (pIter->m_ListPos != NULL) {
                TBLENTRY *pEntry = (TBLENTRY *) DLL_GetAt(pIter->m_ListPos, pIter->m_pTable->m_ppList[pIter->m_Index]);
                *key = pEntry->key;
                return pEntry->data;   /* return the tail element of the previous list that was not empty */
            }
        }
    }
    return NULL;                       /* return null if there is no previous element */
}


/* create a String Map hash table */

MapStrToPtr *SM_Create(int nSize)
{
    int i;
    MapStrToPtr *Map = (MapStrToPtr *) malloc(sizeof(MapStrToPtr));
    Map->m_Size = nSize;
    /* allocate an array of DLLIST* of length nSize */
    Map->m_ppList = (DLLIST **) malloc(sizeof(DLLIST *) * Map->m_Size);
    for (i = 0; i < Map->m_Size; i++)  /* create the lists */
        Map->m_ppList[i] = DLL_Create();
    Map->m_Count = 0;
    return Map;
}


/* deletes a String Map hash table */

void SM_Delete(MapStrToPtr * Map)
{
    int i;
    for (i = 0; i < Map->m_Size; i++) {
        DLLPOS pos = DLL_GetHeadPosition(Map->m_ppList[i]);
        while (pos != NULL) {          /* delete each table entry from each list */
            TBLENTRY *pEntry = (TBLENTRY *) DLL_GetNext(&pos, Map->m_ppList[i]);
            DeleteTblEntry(pEntry);
        }
        DLL_RemoveAll(Map->m_ppList[i]);        /* remove nodes from each list */
        DLL_Delete(Map->m_ppList[i]);  /* delete the list structure */
    }
    free(Map->m_ppList);
    /* free the list array */
    free(Map);
}


/* find element that is associated with key */

void *SM_Lookup(char *key, MapStrToPtr * Map)
{
    TBLENTRY *pEntry = SM_LookupEntry(key, Map);
    if (pEntry != NULL)
        return pEntry->data;
    return NULL;
}


/* find table entry that is associated with key */

TBLENTRY *SM_LookupEntry(char *key, MapStrToPtr * Map)
{
    int idx = SM_Hash(key, Map);        /* get the list array index where the element is located */
    DLLPOS pos = DLL_GetHeadPosition(Map->m_ppList[idx]);
    while (pos != NULL) {              /* search for the element */
        TBLENTRY *pEntry = (TBLENTRY *) DLL_GetNext(&pos, Map->m_ppList[idx]);
        if (!strcmp(pEntry->key, key)) {
            return pEntry;
        }
    }
    return NULL;
}


/* maps data to key */

void *SM_Map(char *key, void *data, MapStrToPtr * Map)
{
    int idx = SM_Hash(key, Map);        /* get the list array index of the element */
    void *ret;
    TBLENTRY *pEntry = SM_LookupEntry(key, Map);        /* determine that the key is not already in the hash table */
    if (pEntry == NULL) {              /* if no other items match this key */
        pEntry = NewTblEntry(key, data);        /* add the entry the list */
        DLL_AddHead(pEntry, Map->m_ppList[idx]);
        Map->m_Count++;
        ret = NULL;
    } else {                           /* otherwise replace the current key's data with new data */

        ret = pEntry->data;
        pEntry->data = data;

    }
    return ret;
}


/* remove a key from the hash table */

void *SM_RemoveKey(char *key, MapStrToPtr * Map)
{
    int idx = SM_Hash(key, Map);
    DLLPOS pos = DLL_GetHeadPosition(Map->m_ppList[idx]);
    while (pos != NULL) {
        DLLPOS prevpos = pos;
        TBLENTRY *pEntry = (TBLENTRY *) DLL_GetNext(&pos, Map->m_ppList[idx]);
        if (!strcmp(pEntry->key, key)) {
            void *ret;
            DLL_RemoveAt(prevpos, Map->m_ppList[idx]);
            ret = pEntry->data;
            DeleteTblEntry(pEntry);
            Map->m_Count--;
            return ret;
        }
    }
    return NULL;
}



/* calculates a hash value based on a string */

unsigned int SM_Hash(char *key, MapStrToPtr * Map)
{
    unsigned int hashval = 0;
    short up3 = 0;
    while (*key != '\0') {
        up3 = hashval >> 29;
        hashval <<= 3;
        hashval += up3 + *key;
        key++;
    }
    return (hashval % Map->m_Size);
}
