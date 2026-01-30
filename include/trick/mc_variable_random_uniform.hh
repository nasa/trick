/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to support generation and assignment
           of a random value distributed uniformally.
           Provides float and integer distributions)

LIBRARY DEPENDENCY:
  (../src/mc_variable_random_uniform.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_RANDOM_UNIFORM_HH
#define CML_MONTE_CARLO_VARIABLE_RANDOM_UNIFORM_HH

#include "mc_variable_random.hh"

#include <random>

/*****************************************************************************
MonteCarloVariableRandomUniform
Purpose:()
*****************************************************************************/
class MonteCarloVariableRandomUniform : public MonteCarloVariableRandom
{
protected:
#ifndef SWIG
    std::uniform_real_distribution<double> distribution;
#endif

public:
    MonteCarloVariableRandomUniform(const std::string & var_name,
                                    unsigned int seed = 0,
                                    double lower_bound = 0.0,
                                    double upper_bound = 1.0);

    virtual ~MonteCarloVariableRandomUniform() {}

    virtual void generate_assignment();
    virtual std::string summarize_variable() const;

private: // and undefined:
    MonteCarloVariableRandomUniform(const MonteCarloVariableRandomUniform &);
    MonteCarloVariableRandomUniform & operator=(const MonteCarloVariableRandomUniform &);
};

/*****************************************************************************
MonteCarloVariableRandomUniformInt
Purpose:()
*****************************************************************************/
class MonteCarloVariableRandomUniformInt : public MonteCarloVariableRandom
{
protected:
#ifndef SWIG
    std::uniform_int_distribution<int> distribution;
#endif

public:
    MonteCarloVariableRandomUniformInt(const std::string & var_name,
                                       unsigned int seed = 0,
                                       double lower_bound = 0,
                                       double upper_bound = 1);

    virtual ~MonteCarloVariableRandomUniformInt() {}

    virtual void generate_assignment();
    virtual std::string summarize_variable() const;

private: // and undefined:
    MonteCarloVariableRandomUniformInt(const MonteCarloVariableRandomUniformInt &);
    MonteCarloVariableRandomUniformInt & operator=(const MonteCarloVariableRandomUniformInt &);
};
#endif
