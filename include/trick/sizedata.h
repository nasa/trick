/* This is an attempt at keeping data sizes consistent across machines
by defining a INT8,INT16, and INT32 type for use as 8,16,and 32 bit
integers.  These won't work well with i/o like printf, but are useful
for transfering binary data from one machine to another. */

#ifndef SIZEDATA_H
#define SIZEDATA_H
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __sun
#if UCHAR_MAX==255
typedef char INT8;
typedef unsigned char UINT8;
#endif
#else
#if UCHAR_MAX==255U
typedef char INT8;
typedef unsigned char UINT8;
#endif
#endif

#if SHRT_MAX==32767
typedef short INT16;
typedef unsigned short UINT16;
#elif INT_MAX==32767
typedef int INT16;
typedef unsigned int UINT16;
#endif

#if SHRT_MAX==2147483647
typedef short INT32;
typedef unsigned short UINT32;
#elif INT_MAX==2147483647
typedef int INT32;
typedef unsigned int UINT32;
#elif LONG_MAX==2147483647
typedef long INT32;
typedef unsigned long UINT32;
#endif

#ifdef __cplusplus
}
#endif

#endif


























