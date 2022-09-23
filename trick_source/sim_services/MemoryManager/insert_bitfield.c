/*
   PURPOSE: (Insert a value into a bitfield.)

   REFERENCE: ((meow) (woof))

   PROGRAMMERS: (((Jane Falgout) (LinCom)) ((Alex Lin) (NASA) (8/00))) */

#include "trick/bitfield_proto.h"

unsigned int insert_bitfield_any(unsigned int bitfield, /* In: Bitfiled to insert value into */
                                 int value,     /* In: Value */
                                 int size,      /* In: Declared size of bitfield */
                                 int start,     /* In: Starting bit */
                                 int bits)
{                                      /* In: Number of bits in bitfield */

    unsigned int mask;
    static int numbits = sizeof(unsigned int) * 8;

    union {
        long l;
        char c[sizeof(long)];
    } un;

    un.l = 1;                          /* Used for Little/Big Endian Detection */

    /* Mask out sign extension bits in case the value is negative */
    value &= (0xFFFFFFFF >> (32 - bits));

    /*
     * Create a mask for the purpose of clearing the previous value of
     * the bit field parameter for which an input was received, while
     * preserving the other values in the underlying memory location
     * shared by other bit field parameters.  Do a bitwise AND of this
     * mask with the underlying bit field memory location.  The input
     * value has been copied into an unsigned int.  Shift the bits of
     * that unsigned int the appropriate number of bits so that the bit
     * position of the input value withing the unsigned int maps to the
     * bit position at which it needs to be stored in the underlying bit
     * field memory location. Do a bitwise OR of the shifted value with
     * the underlying bit field memory location, thus replacing the
     * previous value of the input bit field parameter with the input
     * value while preserving the values of other bit field parameters
     * that share the same underlying memory location.
     * On big endian architectures, the most significant byte (MSB) of an
     * integer is the first byte (i.e, the byte at the starting memory
     * location of the integer).  On little endian architectures. the
     * least significant byte (LSB) is the first byte.  The manner in
     * which bit fields are packed into a structure is not defined in
     * the ANSI standard, but what has been observed on several
     * platforms is that the bit field values are stored in the order in
     * which they were declared, with the first bit field starting at
     * the first byte.  Therefore, the first bit field of a structure
     * on a big endian machine is stored at the high order end of its
     * memory location; on a little endian machine, the first bit field
     * is stored at the low order end of its memory location.  These
     * storage differences influence the construction of the bit field
     * mask and the number of bits by which the input value is shifted.
     *
     * Keith says that the comment above might be the longest
     * in Trick source code.  I haven't read it.  It might even
     * be longer than the sum total of all other comments.
     * When the comments are longer than the code it worries me.
     */

    if (un.c[sizeof(long) - 1] == 1) {
        /* Big endian */

        if (start == 0)
            mask = 0x00000000;
        else
            mask = 0xFFFFFFFF >> (numbits - start);
        if (start + bits < 32)
            mask |= (0xFFFFFFFF << (start + bits));
        bitfield &= mask;
        bitfield |= (value << start);
    } else {
        /* Little endian */

        /* if bitfield is smaller than 32 bits, adjust start accordingly */
        if (size == sizeof(short)) {
            start += 16;
        } else if (size == sizeof(char)) {
            start += 24;
        }

        if (start == 0)
            mask = 0x00000000;
        else
            mask = 0xFFFFFFFF << (numbits - start);
        if (start + bits < 32)
            mask |= (0xFFFFFFFF >> (start + bits));

        bitfield &= mask;
        bitfield |= (value << (32 - (start + bits)));
    }

    return (bitfield);
}
