/* implementation of record array */

#include "trick/record_array.h"
#include <stdlib.h>


RecordArray *Array_Create(void)
{
    RecordArray *ra = (RecordArray *) malloc(sizeof(RecordArray));
    ra->RecordSize = 1;
    ra->RecordCount = 0;
    ra->BufferSize = (INT32 *) malloc(sizeof(INT32));
    *(ra->BufferSize) = sizeof(INT32);
    ra->Buffer = NULL;
    return ra;
}



void *Array_AddRecord(RecordArray * pArray)
{
    pArray->RecordCount++;
    if ((int) (pArray->RecordSize * pArray->RecordCount + sizeof(INT32)) > *(pArray->BufferSize)) {
        int newsize = sizeof(INT32) + (pArray->RecordCount + 10) * pArray->RecordSize;
        pArray->BufferSize = (INT32 *) realloc(pArray->BufferSize, (size_t) newsize);
        *(pArray->BufferSize) = newsize;
        pArray->Buffer = pArray->BufferSize + 1;
    }
    return ((char *) pArray->Buffer) + pArray->RecordSize * (pArray->RecordCount - 1);
}


void *Array_GetAt(int nRecord, RecordArray * pArray)
{
    if (nRecord >= pArray->RecordCount) {
        pArray->RecordCount = nRecord + 1;
        if (*(pArray->BufferSize) < (int) (sizeof(INT32) + pArray->RecordCount * pArray->RecordSize)) {
            int newsize = (pArray->RecordCount + 10) * pArray->RecordSize + sizeof(INT32);
            pArray->BufferSize = (INT32 *) realloc(pArray->BufferSize, (size_t) newsize);
            *(pArray->BufferSize) = newsize;
            pArray->Buffer = pArray->BufferSize + 1;
        }
    }
    return ((char *) pArray->Buffer) + pArray->RecordSize * nRecord;
}


int Array_GetCount(RecordArray * pArray)
{
    return pArray->RecordCount;
}


void Array_SetCount(int nRecords, RecordArray * pArray)
{

    pArray->RecordCount = nRecords;
    if (*(pArray->BufferSize) < (int) (sizeof(INT32) + pArray->RecordCount * pArray->RecordSize)) {
        int newsize = (pArray->RecordCount + 10) * pArray->RecordSize + sizeof(INT32);
        pArray->BufferSize = (INT32 *) realloc(pArray->BufferSize, (size_t) newsize);
        *(pArray->BufferSize) = newsize;
        pArray->Buffer = pArray->BufferSize + 1;
    }
}


void Array_SetRecordSize(int size, RecordArray * pArray)
{
    pArray->RecordSize = size;
}


int Array_GetBufferSize(RecordArray * pArray)
{
    return *(pArray->BufferSize);
}


int Array_GetRecordSize(RecordArray * pArray)
{
    return pArray->RecordSize;
}



void *Array_GetBuffer(RecordArray * pArray)
{
    return pArray->Buffer;
}


void *Array_GetBufferWithSize(RecordArray * pArray)
{
    return pArray->BufferSize;
}
