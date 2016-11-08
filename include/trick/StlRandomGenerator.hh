/*
  PURPOSE:                     (Abstract Base Class for subclass object to hold a
                                Pseudo-Random Number engine and associated distribution)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Matt Jessick) (LZT) (10/2014))
*/

#ifndef STLRANDOMGENERATOR_HH
#define STLRANDOMGENERATOR_HH


// TEMPORARY placement for standalone /////////////////

#if 0
        // from rand_generator   TRICK_GSL_TYPE
        typedef enum {
            TRICK_GSL_GAUSS,
            TRICK_GSL_FLAT,
            TRICK_GSL_POISSON
        } TRICK_GSL_TYPE;


        typedef union {

            unsigned int ui ;     /* -- unsigned integer return */
            double d;             /* -- double return */
            unsigned long long ll ;  /* -- long long field used for printing */
            int ii;               /* -- signed int return */

        } TRICK_GSL_RETURN_TYPE;
#endif

// ////////////////////////////////////

#include "trick/rand_generator.h"

#if (defined(_HAVE_TR1_RANDOM) || defined(_HAVE_STL_RANDOM))
#include <random>

// The template types of the std::poisson_distribution differ between c++0x and c++11
#if (defined(_HAVE_TR1_RANDOM) && !defined(STL_POISSON_TEMPLATE_TYPES))
#define STL_POISSON_TEMPLATE_TYPES int, double
#endif

#if (defined(_HAVE_STL_RANDOM) && !defined(STL_POISSON_TEMPLATE_TYPES))
#define STL_POISSON_TEMPLATE_TYPES int
#endif

#endif

class StlRandomGeneratorFactory;


///@brief Abstract Base class for object that contains a specified <random> engine and distribution together
class StlRandomGenerator
{
public:

    /**@brief A random number generation engine type enum.
        Used for TR1 and C++11 random number generation facilities
    */
    ///@warning with GCC 4.4.7, -std=c++0x, MINSTD_RAND_ENGINE,MT19937_ENGINE
    ///         'engines' return pseudo-random numbers outside the cannonical range of 0 <= x < 1.
    ///
    ///                 This causes an __INFINITE LOOP__ within the std::normal_distribution.
    ///
    ///
    ///         For TR1 compilers with this problem, use RANLUX_BASE_01_ENGINE or RANLUX_64_BASE_01_ENGINE
    ///         which return within the cannonical range.
    ///
    ///@note when adding new engine types, coordinate values with MonteVarRandom.
    enum StlEngine {
        TRICK_DEFAULT_ENGINE     = 1, /**< -- std::ranlux_base_01 for TR1, std::mt19937 for _HAVE_STD_RANDOM */

#ifdef _HAVE_TR1_RANDOM
        RANLUX_BASE_01_ENGINE    = 2, /**< -- TR1 only std::ranlux_base_01 Engine, note: replaced for c++11 */
        RANLUX_64_BASE_01_ENGINE = 3, /**< -- TR1 only std::ranlux64_base_01 Engine, note: replaced for c++11 */
#endif

#ifdef _HAVE_STL_RANDOM
        // NOTE: MINSTD_RAND_ENGINE and MT19937_ENGINE return out of canonical range
        // for (at least) GCC 4.4.7,so are not provided for _HAVE_TR1_RANDOM
        MINSTD_RAND_ENGINE       = 2, /**< -- std::minstd_rand Minimal Standard Linear Congruential Engine */
        MT19937_ENGINE           = 3, /**< -- std::mt19937 Mersenne Twister Engine */

        MT19937_64_ENGINE        = 4, /**< -- std::mt19937_64  64 bit Mersenne Twister Engine. Not available TR1 */

        RANLUX_24_BASE_ENGINE    = 5, /**< -- std::ranlux24_base Engine */
        RANLUX_44_BASE_ENGINE    = 6, /**< -- std::ranlux48_base Engine */

        RANLUX_24_ENGINE         = 7, /**< -- std::ranlux24 Engine */
        RANLUX_44_ENGINE         = 8, /**< -- std::ranlux48 Engine */

        KNUTH_B_ENGINE           = 9 /**< -- std::knuth_b Engine */
#endif
    };

    /** A random distribution. */
    enum StlDistribution {
        GAUSSIAN = TRICK_GSL_GAUSS,  /**< -- Gaussian distribution */
        FLAT     = TRICK_GSL_FLAT,   /**< -- flat distribution */
        POISSON  = TRICK_GSL_POISSON /**< -- Poisson distribution */
    };

    ///@param in_param_a first parameter in distribution::param_type
    ///                  min for FLAT, mean for GAUSSIAN and POISSON
    ///
    ///@param in_param_b second parameter in distribution::param_type
    ///                  max for FLAT, std deviation for GAUSSIAN, (unused for POISSON)
    ///
    ///@param in_seed         starting seed for random number engine
    ///@param in_engine_type  Engine type enumeration for random number engine
    ///@param in_dist_type    Distribution type enumeration for random number distribution
    ///
    explicit StlRandomGenerator(double        in_param_a = 0.0,
                                double        in_param_b = 1.0,
                                unsigned long in_seed    = 12345,
                                StlRandomGenerator::StlDistribution in_dist_type   = StlRandomGenerator::FLAT,
                                StlRandomGenerator::StlEngine       in_engine_type = StlRandomGenerator::TRICK_DEFAULT_ENGINE
                                );

    virtual ~StlRandomGenerator();

    ///@brief return next pseudo-random number
    ///@note POISSON distribution returns an int type,
    ///  so this would need to either cast to double or return a union class like
    ///  the Trick rand routines already do in order to fit in.
    virtual TRICK_GSL_RETURN_TYPE operator()() = 0;

    ///@brief reset seed for pseudo-random number engine
    virtual void set_seed(unsigned long in_seed) = 0;

    ///@brief reset parameters for the distribution
    ///@param a is min for FLAT, mean for GAUSSION and POISSON
    ///@param b is max for FLAT, sigma for GAUSSION and unused for POISSON
    ///@note For TR1, set_param replaces the distribution via operator=
    virtual void set_param(double a, double b = 0.0) = 0;

    double get_param_a() const { return param_a; }
    double get_param_b() const { return param_b; }

protected:

    double           param_a;        /**< -- first param in distribution::param_type, min for FLAT, mean for GAUSSIAN and POISSON */
    double           param_b;        /**< -- second param in distribution::param_type, max for FLAT, std deviation for GAUSSIAN, (unused for POISSON) */
    int              initialSeed;    /**< -- starting seed */

    StlEngine        engineEnum;     /**< -- STL random number engine type enumeration */
    StlDistribution  distEnum;       /**< -- STL random number distribution type enumeration */
};

///@brief Factory for StlRandomGenerator objects
class StlRandomGeneratorFactory {
public:

    ///@brief construct a new StlRandomGenerator object on the heap with the input characteristics
    static StlRandomGenerator* newGenerator(
        double        in_param_a, /**< -- first param in distribution::param_type, min for FLAT, mean for GAUSSIAN and POISSON */
        double        in_param_b, /**< -- second param in distribution::param_type, max for FLAT, std deviation for GAUSSIAN, (unused for POISSON) */
        unsigned long seed,       /**< -- starting seed */
        StlRandomGenerator::StlDistribution in_dist_type,   /**< -- STL random number distribution type enumeration */
        StlRandomGenerator::StlEngine       in_engine_type = StlRandomGenerator::TRICK_DEFAULT_ENGINE /**< -- STL random number engine type enumeration */
        );

protected:
    StlRandomGeneratorFactory()  { }
    ~StlRandomGeneratorFactory() { }
};



#if (defined(_HAVE_TR1_RANDOM) || defined(_HAVE_STL_RANDOM))

// include StlRandomGenerator subclass
#include "trick/StlRandomGeneratorSub.hh"


#else
    // do nothing class may be needed if no <random> ??
    // may not if the base class is always a pointer (nullpointer) if not instantiated.

#endif // _HAVE_TR1_RANDOM or _HAVE_STL_RANDOM


#endif // StlRandomGenerator_EXISTS
