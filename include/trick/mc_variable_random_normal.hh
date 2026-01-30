/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to support generation and assignment
           of a random value distributed normally.)

LIBRARY DEPENDENCY:
  (../src/mc_variable_random_normal.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_RANDOM_NORMAL_HH
#define CML_MONTE_CARLO_VARIABLE_RANDOM_NORMAL_HH

#include <random>

#include "mc_variable_random.hh"

class MonteCarloVariableRandomNormal : public MonteCarloVariableRandom
{
public:
    enum TruncationType
    {
        StandardDeviation,
        Relative,
        Absolute
    };

    size_t max_num_tries;

protected:
#ifndef SWIG
    std::normal_distribution<double> distribution;
#endif
    double min_value;
    double max_value;
    bool truncated_low;
    bool truncated_high;

public:
    MonteCarloVariableRandomNormal(const std::string & var_name,
                                   unsigned int seed = 0,
                                   double mean = 0,
                                   double stdev = 1);

    virtual ~MonteCarloVariableRandomNormal() {}

    virtual void generate_assignment();
    virtual std::string summarize_variable() const;
    void truncate(double limit, TruncationType type = StandardDeviation);
    void truncate(double min, double max, TruncationType type = StandardDeviation);
    void truncate_low(double limit, TruncationType type = StandardDeviation);
    void truncate_high(double limit, TruncationType type = StandardDeviation);
    void untruncate();

private: // and undefined:
    MonteCarloVariableRandomNormal(const MonteCarloVariableRandomNormal &);
    MonteCarloVariableRandomNormal & operator=(const MonteCarloVariableRandomNormal &);
};
#endif
