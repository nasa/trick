
#ifndef RECORD_ARRAY_H
#define RECORD_ARRAY_H

#include "trick/sizedata.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int RecordSize;
    int RecordCount;
    INT32* BufferSize;
    void* Buffer;
}RecordArray;



   /* create and initialize a record array */
RecordArray* Array_Create(void);

   /* Appends a record to the end of the array (allocating memory if necessary)
    * And returns a pointer to the start of that record
    */

void* Array_AddRecord(RecordArray* pArray);


   /* Gets a pointer to the record stored at the zero based record index given by nRecord.
    * If nRecord is greater than the record count - 1, realloc is called to make the
    * record buffer larger
    */

void* Array_GetAt(int nRecord,RecordArray* pArray);


   /* Returns the number of records in the buffer */

int Array_GetCount(RecordArray* pArray);

   /* Allocates memory if necessary to contain the number of records given by nRecords */

void Array_SetCount(int nRecords,RecordArray* pArray);


   /* Sets the size of the record being stored */
void Array_SetRecordSize(int size,RecordArray* pArray);



   /* returns the size of the buffer in bytes */
int Array_GetBufferSize(RecordArray* pArray);


   /* returns the size of the record being stored */
int Array_GetRecordSize(RecordArray* pArray);


   /* returns a pointer to the start of the buffer */
void* Array_GetBuffer(RecordArray* pArray);


   /* returns a pointer to the buffer prefixed with an INT32 value which holds the
      record count
    */

void* Array_GetBufferWithSize(RecordArray* pArray);


#ifdef __cplusplus
}
#endif


#endif







