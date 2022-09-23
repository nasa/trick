#ifndef MAPSTRTOPTR_H
#define MAPSTRTOPTR_H


/************************************
PURPOSE:  This data structure uses a hash table to map strings to pointers.  Common uses of this type of
          structure could be to store the address of named references.  This could be using a filename
          to reference the data stored in a file that has been previously read in.  Essentially, any
          piece of data that needs to be associated with a string, and then accessed using this string
          can be efficiently accessed using this data structure.  The string map is analogous to the
          way numbers are used to access elements of an array by using the number as an index like
          data=myarray[number];  This data structure gives you the ability to do access data in a
          similar way: data=SM_Map("mydata",mymap);   The advantage of this abstract data type over
          other types like arrays is, as elements are added to the map, the
          time required to retrieve an element from the map will increase much less than O(n) as in
          the case of can array.  An alternative data structure that may give better performance in
          some circumstances is the binary tree.
**************************************************/

#include <stdlib.h>
#include "trick/dllist.h"

#ifdef __cplusplus
extern "C" {
#endif


/* private data structure used in the string to pointer map */

typedef struct _TBLENTRY
{
    char* key;     /* pointer to the key */
    void* data;    /* pointer to the data associated with this key */
}TBLENTRY;


/* create a new TBLENTRY structure with the key being parmKey and data set to parmData */

TBLENTRY* NewTblEntry(char* parmKey, void* parmData);
void DeleteTblEntry(TBLENTRY* entry);




/* maps null-terminated strings to pointers */

typedef struct _MapStrToPtr
{
    /* do not modify the value of these map properties */
    DLLIST** m_ppList;           /* table of linked lists */
    int m_Size;                  /* number of indexes in the hash table */
    int m_Count;                 /* number of elements stored in table */
}MapStrToPtr;


/*************** public functions *****************/

/* create and initialize a table that maps strings to pointers.
   The size specifies the number of lists maintained by the map.  For maximum efficiency use a prime
   number for nSize that is close to the average number of elements being stored in the table.
   The string to pointer map can store an arbitrary number of elements with the maximum size
   being limited by the computer's memory
*/

MapStrToPtr* SM_Create(int nSize);



/* delete a map that associates strings with pointers
   this does not free the user-defined data that the table is storing.. enumerate through the table and
   remove all user-defined data first
*/

void SM_Delete(MapStrToPtr* Map);



/* lookup the data that is associated with the string given by key
   return: a pointer to the data, or NULL if the string given by key is not in the map
*/

void* SM_Lookup(char* key, MapStrToPtr* Map);



/* map a string to a pointer.  key is the string that is being mapped to the pointer passed into data
   NOTE: if you call SM_Map with the same key more than once, the data associated with that key will
   correspond to the data passed on the most recent call.
*/

void* SM_Map(char* key, void* data, MapStrToPtr* Map);


/* unmap a string and pointer.  This is the inverse of the map function.  The return value is a pointer to the
   data mapped to the key.  If the key is not found, NULL is returned
*/

void* SM_RemoveKey(char* key, MapStrToPtr* Map);



/* returns the number of elements in the map */

int   SM_GetCount(MapStrToPtr* Map);



/*********** private functions ****************/


/* returns an unsigned integer based on what is stored in key */

unsigned int SM_Hash(char* key, MapStrToPtr* Map);


/* returns a pointer to the entry associated with key */

TBLENTRY* SM_LookupEntry(char* key, MapStrToPtr* Map);



/* the iterator allows you to traverse through all of the elements stored in the table.  The order in which
   the elements are traversed is indeterminant, but each element will only be traversed one time.
*/

typedef struct _StrMapIterator
{
        int m_Index;
        DLLPOS m_ListPos;
        MapStrToPtr* m_pTable;
}StrMapIterator;


/* creates and initializes an iterator.  Pass the map that you want to iterate on as the map parameter
   To re-use this iterator on a different map, call SMI_Attach */

StrMapIterator* SMI_Create(MapStrToPtr* Map);


/* deletes an iterator */
void SMI_Delete(StrMapIterator* iter);

/* attaches a map to this iterator.  Use this function if you are created an iterator statically,
   (by declaring it on the stack) or if you want to re-use an iterator */

void SMI_Attach(MapStrToPtr* Map,StrMapIterator* iter);


/* gets the first element in a map, key will be modified to point to the key at this location.  Do not free
   this key.  In Addition, always call SMI_GetFirst, before calling SMI_GetNext or SMI_GetPrev
   return: pointer to the data stored at this map location */

void * SMI_GetFirst(char** key, StrMapIterator* iter);


/* gets the element after the previous element retrieved.   */
void * SMI_GetNext(char** key, StrMapIterator* iter);

/* gets the element before the previous element retrieved */
void * SMI_GetPrev(char** key, StrMapIterator* iter);

#ifdef __cplusplus
}
#endif

#endif



