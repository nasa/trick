/*
   PURPOSE: (Interface to GSL random number generators)
   REFERENCE: (GNU Scientific Library)
   PROGRAMMERS: ((Alex Lin) (NASA) (May 2004))
 */

#include <stdio.h>
#include <limits.h>
#include <float.h>
#include "trick/rand_generator.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

#if _HAVE_GSL
#include "gsl/gsl_randist.h"
#else
#include "trick/trick_math_proto.h"
#endif

void trick_gsl_rand_init(TRICK_GSL_RANDIST * G)
{                                      /* Inout: Generator parameters */
#if _HAVE_GSL
    G->rng = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(G->rng, G->seed);
#else
    message_publish(MSG_WARNING, "A suitable GSL library was not found, using Trick's built in random number generators\n");
    G->t_rng.seed_1 = G->seed ;
    G->t_rng.seed_trpl = G->seed ;
#endif

    G->uniform = LCG1 ;
    G->sigma_range = 0.0;

    G->type = TRICK_GSL_GAUSS;

    G->sigma = 1.0;
    G->mu = 0.0;

    G->min = -DBL_MAX;
    G->max = DBL_MAX;

    G->rel_min = 1;
    G->rel_max = 1;
    return;
}

void trick_gsl_rand_alloc(TRICK_GSL_RANDIST * G __attribute__((unused)) )
{                                      /* Inout: Generator parameters */
#if _HAVE_GSL
    G->rng = gsl_rng_alloc(gsl_rng_default);
#else
    message_publish(MSG_WARNING,"A suitable GSL library was not found\n") ;
#endif
}

void trick_gsl_rand_seed(TRICK_GSL_RANDIST * G)
{                                      /* Inout: Generator parameters */
#if _HAVE_GSL
    gsl_rng_set(G->rng, G->seed);
#else
    G->t_rng.seed_1 = G->seed ;
    G->t_rng.seed_trpl = G->seed ;
#endif
}

int trick_gsl_rand(             /* Return: status */
                      TRICK_GSL_RANDIST * G,    /* Inout: Generator parameters */
                      TRICK_GSL_RETURN_TYPE * R)
{                                      /* Inout: Generator parameters */
    double min, max;

    /* Check to see if min and max cutoffs make sense */
    if (G->min >= G->max) {
        message_publish(MSG_ERROR, "trick_gsl_rand(%s:%d): min (%f) must be less than max (%f)\n",
                        __FILE__, __LINE__, G->min, G->max);
        return (-1);
    }

    min = (G->rel_min) ? (G->mu + G->min) : G->min;
    max = (G->rel_max) ? (G->mu + G->max) : G->max;

#ifndef _HAVE_GSL
    G->t_rng.sigma_range = G->sigma_range ;
    G->t_rng.uniform = G->uniform ;
#endif

    unsigned loops = 0;
    switch (G->type) {
        case TRICK_GSL_GAUSS:
            do {
                if (loops++ == 1000) {
                    message_publish(MSG_ERROR, "trick_gsl_rand(%s:%d): Gaussian distribution is unable to produce a random number between %f and %f"
                                    " with mu = %f and sigma = %f\n", __FILE__, __LINE__, min, max, G->mu, G->sigma);
                      return -1;
                }
#if _HAVE_GSL
                R->d = G->mu + gsl_ran_gaussian(G->rng, G->sigma);
#else
                R->d = G->mu + gauss_rnd_bell(&G->t_rng) * G->sigma;
#endif
            } while (R->d < min || R->d > max);
            break;

        case TRICK_GSL_FLAT:
#if _HAVE_GSL
            R->d = gsl_ran_flat(G->rng, min, max);
#else
            do {
                if (loops++ == 1000) {
                    message_publish(MSG_ERROR,
                                    "Trick's Flat distribution function is unable to produce a random number between %f and %f\n",
                                    min, max);
                      return -1;
                }
                R->d = min + uniform_rnd_1(&G->t_rng) * (max - min);
            } while (R->d < min || R->d > max);
#endif
            break;

        case TRICK_GSL_POISSON:
#if _HAVE_GSL
            R->ui = gsl_ran_poisson(G->rng, G->mu);
#else
            R->ui = 0 ;
            message_publish(MSG_ERROR,"Poisson distributions require the GSL library. Returning 0.\n") ;
#endif
            break;
    }

    return (0);
}


void trick_gsl_rand_shutdown(TRICK_GSL_RANDIST * G __attribute__((unused)) )
{                                      /* Inout: Generator parameters */
#if _HAVE_GSL
    gsl_rng_free(G->rng);
#else
    message_publish(MSG_ERROR,"A suitable GSL library was not found\n") ;
#endif
    return;
}
