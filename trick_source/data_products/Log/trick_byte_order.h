
#ifndef TRICK_BYTE_ORDER_H
#define TRICK_BYTE_ORDER_H

#define TRICK_BIG_ENDIAN    (char)0x00
#define TRICK_LITTLE_ENDIAN (char)0x01

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
