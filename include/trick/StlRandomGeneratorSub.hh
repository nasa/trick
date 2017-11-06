// NOTE: This file is intended to be included into the StlRandomGenerator.hh header
#ifndef STLRANDOMGENERATORSUB_HH
#define STLRANDOMGENERATORSUB_HH

#ifdef _HAVE_STL_RANDOM

///@brief return the correct param_type loaded with the input values
///
///@details Provides a way for a common input interface to return the correct param_type
///         for all implemented distributions, without the calling code having to hardcode the distribution.
///         (Because multiple hardcodes won't work for templated distributions where the
///         initialization signatures aren't all the same.
///
///         Common signature of static wrapper methods for all implemented 'Distribution' types:
///
///                Distribution::param_type param(Distribution&, double, double=0.0)
///
///
///         This allows user to use a single call: dist.param( ParamWrapper::param(dist, a,b) )
///         and the compiler will choose the correct ParamWrapper method to match dist.param's
///         need for a Distribution::param_type class object, based on the type of the 'dist' input argument.
///
///         (Otherwise, it is difficult to use the param_type system without knowing the
///         distribution type at compile time.)
///
class ParamWrapper {
public:
    ///@name Parameter wrappers
    ///@brief These methods return the appropriate param_type per the input distribution (which is otherwise unused).
    ///@note Implement one per distribution implemented
    ///@{

    static std::uniform_real_distribution<double>::param_type
    param(__attribute__((unused)) const std::uniform_real_distribution<double>& dist, double min, double max = 0.0)
    { return std::uniform_real_distribution<double>::param_type(min,max); }

    static std::normal_distribution<double>::param_type
    param(__attribute__((unused)) const std::normal_distribution<double>& dist, double mean, double std_dev = 0.0)
    { return std::normal_distribution<double>::param_type(mean,std_dev); }

    static std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>::param_type
    param(__attribute__((unused)) const std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>& dist, double mean, __attribute__((unused)) double unused = 0.0)
    { return std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>::param_type(mean); }

    ///@}
private:
    ParamWrapper();
    ~ParamWrapper();
};

#endif


#if (defined(_HAVE_TR1_RANDOM) || defined(_HAVE_STL_RANDOM))

///@brief Return the appropriate union bit pattern for each distribution
///@note Implement one for each available distribution type.
class StlReturnWrapper {
public:

#ifdef _HAVE_TR1_RANDOM
    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::uniform_real<double>& dist, double ret_val)
#endif
#ifdef _HAVE_STL_RANDOM
    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::uniform_real_distribution<double>& dist, double ret_val)
#endif
    {
        TRICK_GSL_RETURN_TYPE output;
        output.d = ret_val;
        return output;
    }

    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::normal_distribution<double>& dist, double ret_val)
    {
        TRICK_GSL_RETURN_TYPE output;
        output.d = ret_val;
        return output;
    }

    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>& dist, int ret_val)
    {
        TRICK_GSL_RETURN_TYPE output;
        output.ii = ret_val;
        return output;
    }

};
#endif

///@brief Sub class for object that includes a <random> engine and a distribution
///
///@note partial specialization exists for Distribution = std::poisson_distribution<int>
///      because TR1 doesn't implement param_type system, thus has no way to generally
///      init the class! (No way to use a common 2 parameter interface and ignore the
///      second parameter for single parameter param_type distributions.)
///
///@note The partial specialization is NOT required when c++0x support is dropped.
///
///      Also, TR1 Distributions can not have their parameters changed after construction.
///      This results in the need for template full method specialization to provide the type specific init.
///
template <class Engine, class Distribution>
class StlRandomGeneratorSub : public StlRandomGenerator
{
public:

#ifdef _HAVE_STD_RANDOM  // (Concept doesn't exist in TR1)
    typedef typename Distribution::param_type ParamType;
#endif

    ///@note The input in_engine_type and in_dist_type must match the template parameters.
    ///      This is intended to be assured by using StlRandomGeneratorFactory for construction.
    ///
    explicit StlRandomGeneratorSub(
        double                              in_param_a = 0.0,
        double                              in_param_b = 1.0,
        unsigned long                       in_seed    = 12345,
        StlRandomGenerator::StlDistribution in_dist_type   = FLAT,
        StlRandomGenerator::StlEngine       in_engine_type = TRICK_DEFAULT_ENGINE
    )
    :   StlRandomGenerator(in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type),
        engine(in_seed)
    {
        // (Note: distribution member can't be input initialized in the initializer list
        //        because various distributions have different signatures.
        //
        //      NOTE: For the TR1 implementation, The set_param method
        //            _re-constructs_ the object with the desired parameters.
        //            (TR1 Distributions can only be initialized at construction time.)
        //
        set_param(in_param_a, in_param_b);
    }

    virtual ~StlRandomGeneratorSub() { }

    ///@brief return next pseudo-random number
    virtual TRICK_GSL_RETURN_TYPE operator()()
    {
#if (defined(_HAVE_TR1_RANDOM) || defined(_HAVE_STL_RANDOM))
        return StlReturnWrapper::return_value(distribution, distribution(engine));
#else
        // without either, won't ever be called. But to compile, return zero.
        TRICK_GSL_RETURN_TYPE output;
        output.d = 0.0;
        return output;
#endif
    }

    ///@brief reset seed for underlying uniform pseudo-random number generator
    virtual void set_seed(unsigned long in_seed)
    {
        initialSeed = in_seed;
        engine.seed(initialSeed);
    }

    ///@brief reset parameters for the distribution
    ///
    ///@param a is min for FLAT, mean for GAUSSION and POISSON
    ///@param b is max for FLAT, sigma for GAUSSION and unused for POISSON
    ///
    ///@note For the TR1 implementation, The set_param method
    ///      _re-constructs_ the object with the desired parameters.
    ///
    ///      (C++11 allows changing the parameters on the fly.)
    ///
    virtual void set_param(double a, double b = 0.0)
    {
        param_a = a;
        param_b = b;

#ifdef _HAVE_STL_RANDOM
        // (this param_type interface is not provided in TR1)
        distribution.param( ParamWrapper::param(distribution, a,b) );
#else
        // TR1 implementation must use this hokey
        // full template specialization solution.
        tr1_init_distribution(a, b);
#endif
    }

protected:

    Engine          engine;          /**< -- STL random number engine object */
    Distribution    distribution;    /**< -- STL random number distribution object */

private:

#ifdef _HAVE_TR1_RANDOM
    ///@brief For TR1, replace the default initialized distribution with a new one.
    ///
    ///@details because TR1 only allows initialization of Distribution parameters at
    /// construction time (C++11 provides the param_type system for doing this post-construction),
    /// this method with full template specializations is used to get around this
    /// lack of generality for the few so-far implemented distribution options.
    ///
    /// Once C++11 is required, these can be removed and the simpler C++11 set_param
    /// method used for all.
    ///
    void tr1_init_distribution(double a, double b)
    {
        // All two parameter signature distributions can use this method.
        // Deviant sets must implement a full template specialization for this method.
        distribution = Distribution(a, b);
    }
#endif

};

#ifdef _HAVE_TR1_RANDOM

// implement one of these template FULL specializations
// for each implemented TR1 StlRandomGenerator::StlEngine enums
//
// So far, only the Poisson distribution requires a single parameter,
// causing the issue.

/*
// these engines are no longer allowed for TR1 (caused infinite loops in normal_distribution)

template<>
inline
void
StlRandomGeneratorSub<std::minstd_rand,
                      std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>
                     >::tr1_init_distribution(double a, double __attribute__((unused)) b)
{
    distribution = std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>(a);
}

template<>
inline
void
StlRandomGeneratorSub<std::mt19937,
                      std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>
                     >::tr1_init_distribution(double a, double __attribute__((unused)) b)
{
    distribution = std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>(a);
}

*/

template<>
inline
void
StlRandomGeneratorSub<std::ranlux_base_01,
                      std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>
                     >::tr1_init_distribution(double a, double __attribute__((unused)) b)
{
    distribution = std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>(a);
}

template<>
inline
void
StlRandomGeneratorSub<std::ranlux64_base_01,
                      std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>
                     >::tr1_init_distribution(double a, double __attribute__((unused)) b)
{
    distribution = std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>(a);
}

#endif

#endif
