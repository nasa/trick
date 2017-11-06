/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef MONTEVARRANDOM_HH
#define MONTEVARRANDOM_HH

#include <string>

#include "trick/MonteVar.hh"
#include "trick/rand_generator.h"
#include "trick/StlRandomGenerator.hh"

// This block of code disowns the pointer on the python side so you can reassign
// python variables without freeing the C++ class underneath
#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::MonteVarRandom::MonteVarRandom(std::string name, Distribution distribution) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick {

    /**
     * A variable whose values are taken from a random distribution.
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteVarRandom : public Trick::MonteVar {

        public:
        ///@brief Random number generation engine type.
        ///
        ///@details Used for TR1 and C++11 random number generation facilities
        ///
        ///@note: Values for this type must match those of StlRandomGenerator::StlEngine
        ///
        enum StlEngine {
            NO_ENGINE                = 0, /**< -- This selection causes the Trick hand coded fallback algorithms to be used */

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

            KNUTH_B_ENGINE           = 9  /**< -- std::knuth_b Engine */
#endif
        };

        /** A random distribution. */
        enum Distribution {
            GAUSSIAN = TRICK_GSL_GAUSS, /**< Gaussian distribution */
            FLAT = TRICK_GSL_FLAT,      /**< flat distribution */
            POISSON = TRICK_GSL_POISSON /**< Poisson distribution */
        };

        protected:
        /** Engine. */
        StlEngine engineType; /**< trick_units(--) STL C++11 random number generation engine enum */

        /** Distribution. */
        Distribution distribution; /**< \n trick_units(--) */

        /** Random distribution structure. */
        TRICK_GSL_RANDIST randist; /**< \n trick_units(**) */

        StlRandomGenerator* stlGenPtr; /**< trick_units(**) STL pseudo-random number generator */

        public:
        /**
         * Constructs a MonteVarRandom with the given name, distribution, and units.
         *
         * @param name the fully qualified name of the simulation variable to which this MonteVarRandom refers
         * @param distribution this variable's distribution
         * @param unit this variable's units
         * @param engine this variables's pseudo-random number 'engine'. Defaulted to NO_ENGINE (use Trick algorithms)
         */
        MonteVarRandom(std::string name, Distribution distribution, std::string unit = "", StlEngine engine = MonteVarRandom::NO_ENGINE);

        /**
         * Destructor
         */
        ~MonteVarRandom();

        /**
         * Sets the seed.
         *
         * @param seed the seed
         */
        void set_seed(unsigned long seed);

        /**
         * Sets the standard deviation.
         *
         * @param sigma the standard deviation
         */
        void set_sigma(double sigma);

        /**
         * Sets the sigma range for the Trick RNG.
         *
         * @param sigma_range the range for the output random number
         */
        void set_sigma_range(int sigma_range);

        /**
         * Sets the mean.
         *
         * @param mu the mean
         */
        void set_mu(double mu);

        /**
         * Sets the minimum.
         *
         * @param min the minimum
         */
        void set_min(double min);

        /**
         * Sets the maximum.
         *
         * @param max the maximum
         */
        void set_max(double max);

        /**
         * Sets the relativity of the minimum.
         *
         * @param relativity the relativity
         */
        void set_min_is_relative(bool relativity);

        /**
         * Sets the relativity of the maximum.
         *
         * @param relativity the relativity
         */
        void set_max_is_relative(bool relativity);

        /**
         * Sets the Uniform Random number generator option.
         *
         * @param uniform the RNG option
         */
        void set_uniform_generator(uniform_generator uniform);

        /**
         * @return value of the absolute minimum, taking into account all options (relative or absolute input)
         */
        double get_absolute_min() const { return (randist.rel_min) ? (randist.mu + randist.min) : randist.min; }

        /**
         * @return value of the absolute maximum, taking into account all options (relative or absolute input)
         */
        double get_absolute_max() const { return (randist.rel_max) ? (randist.mu + randist.max) : randist.max; }

        // Describes the properties of this variable.
        std::string describe_variable();

        protected:
        virtual std::string get_next_value();

        /**
         * updates the STL randomization subobject when settings are changed, maintaining consistency
         */
        void updateStlRandom();
    };

};
#endif
