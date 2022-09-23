/*
PURPOSE: (Byte swapping.)
REFERENCES: (Copied from ISP and changed names, added float and long swaps)
PROGRAMMERS: (((Jane Falgout) (LinCom) (5/98) (--) (--)))
ICG: (No)
*/

#ifndef TRICK_BYTESWAP_H
#define TRICK_BYTESWAP_H

#define TRICK_BIG_ENDIAN    (char)0x00
#define TRICK_LITTLE_ENDIAN (char)0x01

#ifdef __cplusplus
extern "C" {
#endif

double trick_byteswap_double(double input);
float trick_byteswap_float(float input);
long trick_byteswap_long(long input);
int trick_byteswap_int(int input);
short trick_byteswap_short(short input);

#ifdef __cplusplus
}
#endif

struct SwapBuffer {
    unsigned int size ;
    char * swap_space ;
} ;

#define TRICK_GET_BYTE_ORDER(IND)   \
{                                   \
   union {                          \
      long    l;                    \
      char    c[sizeof(long)];      \
   } un;                            \
   un.l = 1;                        \
   if (un.c[sizeof(long)-1] == 1)   \
      IND = TRICK_BIG_ENDIAN;       \
   else                             \
      IND =  TRICK_LITTLE_ENDIAN;   \
}
#endif
