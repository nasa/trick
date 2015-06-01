#ifndef BITFIELD_PROTO_H
#define BITFIELD_PROTO_H

#include "trick/attributes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * ADDR_BF is analagous to offsetof for bit field parameters
 * note that it should only be used for integer bit fields
 * addr_bitfield() supports char, short, & int (see ICG)
 */
#define ADDR_BF(X,Y,W) ((offsetof(X,Y)+(sizeof(((X*)0)->Y)+(W*4))))
#define ADDR_BF0(W) (W*4)

/* get a bit field value given its address, declared size, starting bit, & bit length */
#define GET_BITFIELD(addr, size, start, bits) \
(size==sizeof(int)) ? extract_bitfield_any((*(int*)addr), size, start, bits) : \
        (size==sizeof(short)) ? extract_bitfield_any((*(short*)addr), size, start, bits) : \
                extract_bitfield_any((*(char*)addr), size, start, bits)
#define GET_UNSIGNED_BITFIELD(addr, size, start, bits) \
(size==sizeof(int)) ? extract_unsigned_bitfield_any((*(unsigned int*)addr), size, start, bits) : \
        (size==sizeof(short)) ? extract_unsigned_bitfield_any((*(unsigned short*)addr), size, start, bits) : \
                extract_unsigned_bitfield_any((*(unsigned char*)addr), size, start, bits)
/* assign a bit field value given its address, new value, declared size, starting bit, & bit length */
#define PUT_BITFIELD(addr, value, size, start, bits) \
if (size==sizeof(int))  *(int*)addr=insert_bitfield_any((*(unsigned int*)addr), value, size, start, bits) ; \
        else if (size==sizeof(short))  *(short*)addr=(insert_bitfield_any((*(unsigned short*)addr), value, size, start, bits)) ; \
                else  *(char*)addr=(insert_bitfield_any((*(unsigned char*)addr), value, size, start, bits))

#define extract_bitfield( VALUE, START, SIZE ) extract_bitfield_any( VALUE, sizeof(int), START, SIZE )
#define extract_unsigned_bitfield( VALUE, START, SIZE ) extract_unsigned_bitfield_any( VALUE, sizeof(int), START, SIZE )
#define insert_bitfield( BITFIELD, VALUE, START, SIZE ) insert_bitfield_any( BITFIELD, VALUE, sizeof(int), START, SIZE )

         int extract_bitfield_any(int, int, int, int);
unsigned int extract_unsigned_bitfield_any(unsigned int, int, int, int);
unsigned int insert_bitfield_any(unsigned int, int, int, int, int);
        void addr_bitfield(ATTRIBUTES *A, ATTRIBUTES *B, int field_count) ;

#ifdef __cplusplus
}
#endif

#endif

