/*
   PURPOSE: (Uniform random number sequence generator - Triple LCG)

   REFERENCE: (((Vetterling, W. T.) (Tuekolsky, S. A.) (Press, W. H.) (Flannery, B. P.) (Numerical Recipes in C)
   (Cambrige University Press) (Cambrige) (1998)))

   ASSUMPTIONS AND LIMITATIONS: ((to ensure that the table is initialized pass in a negative seed))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((uniform_rnd_triple.o))

   PROGRAMMERS:
    (((Robert Gay) (LinCom) (May 1998) (--) (Trick format))
     ((Robert Gay) (Titan-LinCom) (Aug 2001) (--) (Updates to put routines into trick math)))
 */

#include "trick/rand_generator.h"
#include "trick/trick_math.h"

#define M1  259200
#define IA1 7141
#define IC1 54773
#define RM1 (1.0/M1)
#define M2  134456
#define IA2 8121
#define IC2 28411
#define RM2 (1.0/M2)
#define M3  243000
#define IA3 4561
#define IC3 51349


double uniform_rnd_triple(      /* Return: Random number */
                             RAND_GENERATOR * G)
{                                      /* Inout: Random number seed */

    int j;
    double temp;

    /* Initialize the random number generator on first call or if seed is neg. */
    if (G->seed_trpl < 0 || G->iff == 0) {
        G->iff = 1;
        G->ix1 = ((unsigned int) (IC1 - G->seed_trpl)) % M1;
        G->ix1 = (IA1 * G->ix1 + IC1) % M1;
        G->ix2 = G->ix1 % M2;
        G->ix1 = (IA1 * G->ix1 + IC1) % M1;
        G->ix3 = G->ix1 % M3;

        /* Fill table with numbers from the first two routines */
        for (j = 1; j <= 97; j++) {
            G->ix1 = (IA1 * G->ix1 + IC1) % M1;
            G->ix2 = (IA2 * G->ix2 + IC2) % M2;
            G->table[j] = (G->ix1 + G->ix2 * RM2) * RM1;
        }

        G->seed_trpl = 1;
    }

    /* Generator starts here except when initialized */
    G->ix1 = (IA1 * G->ix1 + IC1) % M1;
    G->ix2 = (IA2 * G->ix2 + IC2) % M2;
    G->ix3 = (IA3 * G->ix3 + IC3) % M3;

    /* Use the third sequence to get an integer between 1 and 97. */
    j = 1 + ((97 * G->ix3) / M3);
    if (j > 97 || j < 1) {
        fprintf(stderr, "trick_utils/math/uniform_rnd_triple.c\n" "Out of table range...");
        return (0);
    }

    temp = G->table[j];                /* Get number out of the table */
    G->table[j] = (G->ix1 + G->ix2 * RM2) * RM1;        /* Refill the table */

    return (temp);
}
