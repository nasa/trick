// NOTE: This file is intended to be included into the StlRandomGenerator.hh header
#ifndef STLRANDOMGENERATORSUB_HH
#define STLRANDOMGENERATORSUB_HH

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

    static std::poisson_distribution<int>::param_type
    param(__attribute__((unused)) const std::poisson_distribution<int>& dist, double mean, __attribute__((unused)) double unused = 0.0)
    { return std::poisson_distribution<int>::param_type(mean); }

    ///@}
private:
    ParamWrapper();
    ~ParamWrapper();
};


///@brief Return the appropriate union bit pattern for each distribution
///@note Implement one for each available distribution type.
class StlReturnWrapper {
public:

    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::uniform_real_distribution<double>& dist, double ret_val)
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

    static TRICK_GSL_RETURN_TYPE return_value(__attribute__((unused)) const std::poisson_distribution<int>& dist, int ret_val)
    {
        TRICK_GSL_RETURN_TYPE output;
        output.ii = ret_val;
        return output;
    }

};

///@brief Sub class for object that includes a <random> engine and a distribution
///
template <class Engine, class Distribution>
class StlRandomGeneratorSub : public StlRandomGenerator
{
public:

    typedef typename Distribution::param_type ParamType;

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
        set_param(in_param_a, in_param_b);
    }

    virtual ~StlRandomGeneratorSub() { }

    ///@brief return next pseudo-random number
    virtual TRICK_GSL_RETURN_TYPE operator()()
    {
        return StlReturnWrapper::return_value(distribution, distribution(engine));
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
    virtual void set_param(double a, double b = 0.0)
    {
        param_a = a;
        param_b = b;

        distribution.param( ParamWrapper::param(distribution, a,b) );
    }

protected:

    Engine          engine;          /**< -- STL random number engine object */
    Distribution    distribution;    /**< -- STL random number distribution object */

};

#endif
