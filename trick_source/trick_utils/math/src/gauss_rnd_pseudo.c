/*
   PURPOSE: (ICDS nomalized gaussian random number generator)

   REFERENCE: ((ICDS))

   ASSUMPTIONS AND LIMITATIONS: ((This is a normalized quasi-gaussian distribution. The base of the curve is 2 *
   sigma-range wide.))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((gauss_rnd_pseudo.o) (uniform_rnd_1.o) (uniform_rnd_triple.o))

   PROGRAMMERS: (((Ray Li) (LMES/DUAL) (96 Nov) (1.0) (ICDS random number generator)) ((Robert Gay) (LinCom) (May 1998)
   (--) (Trick format)) ((Robert Gay) (Titan-LinCom) (Aug 2001) (--) (Updates to put routines into trick math))) */

#include "trick/rand_generator.h"
#include "trick/trick_math.h"

double gauss_rnd_pseudo(        /* Return: random number */
                           RAND_GENERATOR * G)
{                                      /* Inout: generator parameters */
    double sample = 0.0;
    int i;

    switch (G->uniform) {

        case LCG1:                    /* Chose LCG uniform generator */

            for (i = 0; i < 2 * G->sigma_range; i++) {
                sample += uniform_rnd_1(G);
            }
            sample = (sample - (double) G->sigma_range);

            break;


        case TRIPLE:                  /* Chose Triple LCG uniform generator */

            for (i = 0; i < 2 * G->sigma_range; i++) {
                sample += uniform_rnd_triple(G);
            }
            sample = (sample - (double) G->sigma_range);

            break;
    }
    return (sample);
}
