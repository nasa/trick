/*
   PURPOSE: (ICDS nomalized gaussian random number generator - Box Muller)

   REFERENCE: ((ICDS))

   ASSUMPTIONS AND LIMITATIONS: ((This is a normalized gaussian distribution that is bell-shaped. The tails may be
   truncated according to the sigma range desired. Note: If a sigma range is chosen, the code will repeat until a
   number is within the sigma range. Chose 0 if no truncation is desired.))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((gauss_rnd_bell.o) (uniform_rnd_1.o) (uniform_rnd_triple.o))

   PROGRAMMERS: (((Ray Li) (LMES/DUAL) (96 Nov) (1.0) (ICDS random number generator)) ((Robert Gay) (LinCom) (May 1998)
   (--) (Trick format)) ((Robert Gay) (Titan-LinCom) (Aug 2001) (--) (Updates to put routines into trick math))) */

#include "trick/rand_generator.h"
#include "trick/trick_math.h"


double gauss_rnd_bell(          /* Return: Random number */
                         RAND_GENERATOR * G)
{                                      /* Inout: generator parameters */
    double sample = 0.0;
    double x1, x2;

    switch (G->uniform) {

        case LCG1:                    /* Chose single LCG uniform generator */
            if (G->sigma_range < 1) {

                x1 = uniform_rnd_1(G);
                x2 = uniform_rnd_1(G);

                sample = sqrt(-2 * log(x1)) * cos(2 * M_PI * x2);
            }

            else {
                do {
                    x1 = uniform_rnd_1(G);
                    x2 = uniform_rnd_1(G);

                    sample = sqrt(-2 * log(x1)) * cos(2 * M_PI * x2);
                } while (sample < -G->sigma_range || sample > G->sigma_range);
            }

            break;

        case TRIPLE:                  /* Chose Triple LCG uniform generator */
            if (G->sigma_range < 1) {

                x1 = uniform_rnd_triple(G);
                x2 = uniform_rnd_triple(G);

                sample = sqrt(-2 * log(x1)) * cos(2 * M_PI * x2);
            }

            else {
                do {
                    x1 = uniform_rnd_triple(G);
                    x2 = uniform_rnd_triple(G);

                    sample = sqrt(-2 * log(x1)) * cos(2 * M_PI * x2);

                } while (sample < -G->sigma_range || sample > G->sigma_range);
            }

            break;

    }

    return (sample);
}
