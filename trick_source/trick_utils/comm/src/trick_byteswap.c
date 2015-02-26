/******************************************************************************
*                                                                             *
* Trick Simulation Environment Software                                       *
*                                                                             *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                     *
* All rights reserved.                                                        *
*                                                                             *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized   *
* use of Trick Software including source code, object code or executables is  *
* strictly prohibited and LinCom assumes no liability for such actions or     *
* results thereof.                                                            *
*                                                                             *
* Trick Software has been developed under NASA Government Contracts and       *
* access to it may be granted for Government work by the following contact:   *
*                                                                             *
* Contact: Charles Gott, Branch Chief                                         *
*          Flight Robotic Systems Branch                                      *
*          Automation, Robotics, & Simulation Division                        *
*          NASA, Johnson Space Center, Houston, TX                            *
*                                                                             *
******************************************************************************/

/* 
   PURPOSE: (Routines for swapping byte order.  Borrowed from ISP.) 

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((byteswap.o))

   PROGRAMMERS: (((Jane B. Falgout) (LinCom) (08 May 98) (--) (generic HC))) */

/* 
 * $Id: trick_byteswap.c 49 2009-02-02 22:37:59Z lin $
 */


#ifdef STAND_ALONE
#include "../include/trick_byteswap.h"
#else
#include "trick_utils/comm/include/trick_byteswap.h"
#endif

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
