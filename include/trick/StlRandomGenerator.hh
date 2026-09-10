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

#include <random>

///@brief The concrete engine that StlRandomGenerator::TRICK_DEFAULT_ENGINE selects.
///
///@details This is the single source of truth for Trick's default engine. Change it
///         here to re-point TRICK_DEFAULT_ENGINE; the factory follows automatically.
using TrickDefaultEngine = std::mt19937;

class StlRandomGeneratorFactory;


///@brief Abstract Base class for object that contains a specified <random> engine and distribution together
class StlRandomGenerator
{
    public:
        /**@brief A random number generation engine type enum.
            Used for the C++ <random> number generation facilities
        */
        ///@note when adding new engine types, coordinate values with MonteVarRandom.
        enum StlEngine
        {
            TRICK_DEFAULT_ENGINE  = 1, /**< -- whichever engine ::TrickDefaultEngine names */
            MINSTD_RAND_ENGINE    = 2, /**< -- std::minstd_rand Minimal Standard Linear Congruential Engine */
            MT19937_ENGINE        = 3, /**< -- std::mt19937 Mersenne Twister Engine */
            MT19937_64_ENGINE     = 4, /**< -- std::mt19937_64  64 bit Mersenne Twister Engine */
            RANLUX_24_BASE_ENGINE = 5, /**< -- std::ranlux24_base Engine */
            RANLUX_44_BASE_ENGINE = 6, /**< -- std::ranlux48_base Engine */
            RANLUX_24_ENGINE      = 7, /**< -- std::ranlux24 Engine */
            RANLUX_44_ENGINE      = 8, /**< -- std::ranlux48 Engine */
            KNUTH_B_ENGINE        = 9  /**< -- std::knuth_b Engine */
        };

        /** A random distribution. */
        enum StlDistribution
        {
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
        explicit StlRandomGenerator(double in_param_a = 0.0, double in_param_b = 1.0, unsigned long in_seed = 12345,
                                    StlRandomGenerator::StlDistribution in_dist_type = StlRandomGenerator::FLAT,
                                    StlRandomGenerator::StlEngine in_engine_type
                                    = StlRandomGenerator::TRICK_DEFAULT_ENGINE);

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
        virtual void set_param(double a, double b = 0.0) = 0;

        double get_param_a() const { return param_a; }
        double get_param_b() const { return param_b; }

    protected:
        double param_a;  /**< -- first param in distribution::param_type, min for FLAT, mean for GAUSSIAN and POISSON */
        double param_b;  /**< -- second param in distribution::param_type, max for FLAT, std deviation for GAUSSIAN,
                            (unused for POISSON) */
        int initialSeed; /**< -- starting seed */

        StlEngine engineEnum;     /**< -- STL random number engine type enumeration */
        StlDistribution distEnum; /**< -- STL random number distribution type enumeration */
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

// include StlRandomGenerator subclass
#include "trick/StlRandomGeneratorSub.hh"

#endif // StlRandomGenerator_EXISTS
