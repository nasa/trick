/*
   PURPOSE: (Extract an unsigned bitfield.)
   REFERENCE: ((meow) (woof))
   PROGRAMMERS:
     (((Jane Falgout) (Lincom))
      ((Alex Lin) (NASA) (8/00)))
 */

#include "trick/bitfield_proto.h"

unsigned int extract_unsigned_bitfield_any(unsigned int value,  /* In: Value to extract bits from */
                                           int size,    /* In: Declared size of bitfield */
                                           int start,   /* In: Starting bit */
                                           int bits)
{                                      /* In: Number of bits in bitfield */

    int i = 0, j = 0;
    unsigned int mask;
    unsigned int bf = 0;
    union {
        long l;
        char c[sizeof(long)];
    } un;
    un.l = 1;                          /* Used for Little/Big Endian Detection */

    bf = 0;

    if (un.c[sizeof(long) - 1] == 1) {

        /*
         * Big endian
         */

        mask = 0x00000001 << (start + bits - 1);

        for (i = start, j = 0; j < bits; i++, j++) {
            /* First, turn off all bits associated with this bit field within the unsigned integer. Then assign the
               input value to the appropriate bits in the unsigned integer. */
            mask = 0x00000001 << i;
            if ((value & mask) == mask) {
                bf |= mask;
            }
        }
        bf = bf >> start;
    } else {

        /*
         * Little endian
         */

        /* Shift the bit field contents of the current value to the lsb of the underlying int. */
        if (size == sizeof(short)) {
            value = value << 16;
        } else if (size == sizeof(char)) {
            value = value << 24;
        }
        if ((start + bits) < 32) {
            bf = (value) >> (32 - start - bits);
        } else {
            bf = value;
        }

        /* Now, AND the shifted bits with a mask of the appropriate size to obtain the bitfield value. */
        bf &= (0xFFFFFFFF >> (32 - bits));

    }

    return (bf);
}
