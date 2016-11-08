/*
   PURPOSE: (Switching logic for selected random number sequence generator)

   ASSUMPTIONS AND LIMITATIONS: ((Option 0: Uniform - Single Linear Congruential Generator) (Option 1: Uniform - Triple
   Linear Congruential Generator) (Option 2: Normalized Gaussian - Pseudo) (Option 3: Normalized Gaussian - Box Muller
   - Bell shaped) (Note: Options 2 and 3 can use either the single LCG or the triple LCG))

   LIBRARY DEPENDENCY: ((rand_num.o) (uniform_rnd_1.o) (uniform_rnd_triple.o) (gauss_rnd_pseudo.o) (gauss_rnd_bell.o))

   PROGRAMMERS: (((Robert Gay) (LinCom Corp) (May 1998) (--) (--)) ((Robert Gay) (Titan-LinCom) (Aug 2001) (--)
   (Updates to put routines into trick math))) */

#include <stdio.h>
#include "trick/trick_math.h"

double rand_num(RAND_GENERATOR * G)
{                                      /* Inout: Generator parameters */

    switch (G->option) {

        case UNIFORM_RND_1:
            return (uniform_rnd_1(G));
            break;

        case UNIFORM_RND_TRIPLE:
            return (uniform_rnd_triple(G));
            break;

        case GAUSS_RND_PSEUDO:
            return (gauss_rnd_pseudo(G));
            break;

        case GAUSS_RND_BELL:
            return (gauss_rnd_bell(G));
            break;

        case RND_NUM_1:
            return (1);
            break;

        default:
            fprintf(stderr, "Unknown generator option: %d", G->option);
    }

    return (0);
}
