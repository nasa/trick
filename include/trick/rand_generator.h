/*
PURPOSE:
    (Trick Random Number Generator Parameter Definitions)

REFERENCE:
    ((none))

ASSUMPTIONS AND LIMITATIONS:
    ((none))

PROGRAMMERS:
    (((Robert Gay) (LinCom) (June 1998) (--) (--))
     ((Robert Gay) (Titan-LinCom) (Aug 2001) (--) (removed test params)))
*/

#ifndef RAND_GENERATOR_H
#define RAND_GENERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _HAVE_GSL
#include "gsl/gsl_rng.h"
#endif

typedef enum {

    UNIFORM_RND_1,      /* Single Linear Congruential Generator */
    UNIFORM_RND_TRIPLE, /* Triple Linear Congruential Generator */
    GAUSS_RND_PSEUDO,   /* Normalized Quasi-Gaussian Triangular
                               Generator */
    GAUSS_RND_BELL,     /* Normalized Gaussian Bell-Shaped
                               Generator */
    RND_NUM_1           /* Return a value of 1 */
} rand_generator;


typedef enum {

    LCG1,           /* Uniform Single LCG */
    TRIPLE          /* Uniform Triple LCG */
} uniform_generator;


typedef struct {

    rand_generator option;      /* -- Random number generator
                                      option */
    uniform_generator uniform;  /* -- Uniform Random number
                                      generator option */
    unsigned long seed_1;  /* -- Seed for uniform RNG
                                 single LCG */
    long seed_trpl;        /* -- Seed for uniform RNG
                                 triple LCG make this seed
                                 negative for initializing
                                 the RNG table */
    int iff;               /* -- Initialization flag for
                                 triple LCG */
    double table[98];      /* -- Table for RNG triple LCG */
    int sigma_range;       /* -- Sigma range for output random num
                                 Must be > 0 for triangular gauss.
                                 Must be >= 0 for bell gauss,
                                 0 gives infinite sigma. */
    long ix1;              /* -- Intermidiate Radom Number */
    long ix2;              /* -- Intermidiate Radom Number */
    long ix3;              /* -- Intermidiate Radom Number */

} RAND_GENERATOR;

/*
 * Trick's interface to the GNU Scientific Library's
 * random number generators
 */
typedef enum {
    TRICK_GSL_GAUSS,
    TRICK_GSL_FLAT,
    TRICK_GSL_POISSON
} TRICK_GSL_TYPE;

typedef struct {

#ifdef _HAVE_GSL
    gsl_rng *rng;         /* ** GSL's random number generator */
#else
    RAND_GENERATOR t_rng ; /* ** Trick's older random number generator */
#endif
    TRICK_GSL_TYPE type;  /* -- Type of distribution desired */
    unsigned long seed;   /* -- Seed */
    double sigma;         /* -- Std. dev for gauss/poisson distrib */
    double mu;            /* -- mean for gauss/poisson distribution */
    double min;           /* -- Minimum value in range */
    double max;           /* -- Maximum value in range */
    int rel_min;          /* -- Minimun is a relative minimum */
    int rel_max;          /* -- Maximum is a relative maximum */
    int sigma_range;      /* --  Sigma range for output random num
                                 Must be > 0 for triangular gauss.
                                 Must be >= 0 for bell gauss,
                                 0 gives infinite sigma.            */
    uniform_generator uniform; /* -- Uniform Random number generator option */

} TRICK_GSL_RANDIST;

typedef union {

    unsigned int ui ;     /* -- unsigned integer return */
    double d;             /* -- double return */
    unsigned long long ll ;  /* -- long long field used for printing */
    int ii;               /* -- signed int return */

} TRICK_GSL_RETURN_TYPE;

void trick_gsl_rand_init( TRICK_GSL_RANDIST * G) ;
void trick_gsl_rand_alloc( TRICK_GSL_RANDIST * G) ;
void trick_gsl_rand_seed( TRICK_GSL_RANDIST * G) ;
int  trick_gsl_rand( TRICK_GSL_RANDIST * G, TRICK_GSL_RETURN_TYPE * R ) ;
void trick_gsl_rand_shutdown( TRICK_GSL_RANDIST * G) ;

#ifdef __cplusplus
}
#endif
#endif
