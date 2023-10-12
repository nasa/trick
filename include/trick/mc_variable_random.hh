/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to support assignment of
           a random value to a variable based on its type.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_RANDOM_HH
#define CML_MONTE_CARLO_VARIABLE_RANDOM_HH

#include "mc_variable.hh"

#include <random>
#include <string>

/*****************************************************************************
MonteCarloVariableRandom
Purpose:(An intermediate interface class that supports generation of random
         variables of multiple types.  Currently, double, integer, bool, and
         string assignments are supported; others may be added later.)
NOTE - deliberately not using templates here because of the difficulty of having
       SWIG use templates; Monte-Carlo variables are typically created and
       populated via an input-process so the SWIG interface is critical and the
       use of templates a non-starter.
*****************************************************************************/
class MonteCarloVariableRandom : public MonteCarloVariable
{
 protected:
  std::mt19937 random_generator; /* (--)
      the basic random-generator used by all different types of random number
      generators in the <random> library. */
  unsigned int seed_m; /* (--)
      the value used to seed the generator.*/
 public:
  MonteCarloVariableRandom(const std::string & var_name, unsigned int seed = 0): MonteCarloVariable(var_name),random_generator(seed),seed_m(seed) //changed seed member variable to seed_m
  {
    type = MonteCarloVariable::Random;
  }
  virtual ~MonteCarloVariableRandom(){};
  unsigned int get_seed() const {return seed_m;} // override but SWIG cannot process the
                                               // override keyword
 private: // and undefined:
  MonteCarloVariableRandom( const MonteCarloVariableRandom & );
  MonteCarloVariableRandom& operator = (const MonteCarloVariableRandom&);
};
#endif
