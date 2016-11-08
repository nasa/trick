
/*
   PURPOSE: (Routines for swapping byte order.  Borrowed from ISP.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((byteswap.o))

   PROGRAMMERS: (((Jane B. Falgout) (LinCom) (08 May 98) (--) (generic HC))) */


#include "trick/trick_byteswap.h"

/* (note from ISP developers) Normally this routine would just zip through the double a byte at a time and reassign
   them, but the optimizing compiler on the alpha core dumps on this file, after many iterations this "reloading" of
   bytes into the "output" double works with the alpha */
double trick_byteswap_double(   /* Return: Swapped byte */
                                double input)
{                                      /* In: Bytes to be swapped */

    double output;
    unsigned char *out = (unsigned char *) &output;

    *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 7);
    *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input) + 6);
    *(((unsigned char *) &output) + 2) = *(((unsigned char *) &input) + 5);
    *(((unsigned char *) &output) + 3) = *(((unsigned char *) &input) + 4);

    out += 4;
    *(((unsigned char *) out)) = *(((unsigned char *) &input) + 3);
    *(((unsigned char *) out) + 1) = *(((unsigned char *) &input) + 2);
    *(((unsigned char *) out) + 2) = *(((unsigned char *) &input) + 1);
    *(((unsigned char *) out) + 3) = *(((unsigned char *) &input));

    return (output);
}

float trick_byteswap_float(     /* Return: Swapped bytes */
                              float input)
{                                      /* In: Bytes to be swapped */
    float output;

    *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 3);
    *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input) + 2);
    *(((unsigned char *) &output) + 2) = *(((unsigned char *) &input) + 1);
    *(((unsigned char *) &output) + 3) = *(((unsigned char *) &input));

    return (output);
}

long trick_byteswap_long(       /* Return: Swapped byte */
                            long input)
{                                      /* In: Bytes to be swapped */

    long output;
    unsigned char *out = (unsigned char *) &output;

    if (sizeof(long) == 4) {
        /* long is 4 bytes */
        *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 3);
        *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input) + 2);
        *(((unsigned char *) &output) + 2) = *(((unsigned char *) &input) + 1);
        *(((unsigned char *) &output) + 3) = *(((unsigned char *) &input));
    } else {
        /* long is 8 bytes (e.g., dec alpha) */

        *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 7);
        *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input) + 6);
        *(((unsigned char *) &output) + 2) = *(((unsigned char *) &input) + 5);
        *(((unsigned char *) &output) + 3) = *(((unsigned char *) &input) + 4);

        out += 4;
        *(((unsigned char *) out)) = *(((unsigned char *) &input) + 3);
        *(((unsigned char *) out) + 1) = *(((unsigned char *) &input) + 2);
        *(((unsigned char *) out) + 2) = *(((unsigned char *) &input) + 1);
        *(((unsigned char *) out) + 3) = *(((unsigned char *) &input));
    }

    return (output);
}

int trick_byteswap_int(         /* Return: Swapped bytes */
                          int input)
{                                      /* In: Bytes to be swapped */
    int output;

    *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 3);
    *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input) + 2);
    *(((unsigned char *) &output) + 2) = *(((unsigned char *) &input) + 1);
    *(((unsigned char *) &output) + 3) = *(((unsigned char *) &input));
    return (output);
}

short trick_byteswap_short(     /* Return: Swapped bytes */
                              short input)
{                                      /* In: Bytes to be swapped */
    short output;

    *(((unsigned char *) &output)) = *(((unsigned char *) &input) + 1);
    *(((unsigned char *) &output) + 1) = *(((unsigned char *) &input));
    return (output);
}
