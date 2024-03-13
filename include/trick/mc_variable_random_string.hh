/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to randomly pick one of a set of
           character strings.  These strings could represent actual string
           variables, or enumerated types, or commands, or any other concept
           that can be expressed as an assignment.)

ASSUMPTIONS: (The content of the selected string will be assigned as written.
              Consequently, if the value is being assigned to an actual string
              or char variable, the contents of the string should be enclosed
              in quotes.
              E.g. the string might look like: "'actual_string'" so that the
              assignment would look like
                variable = 'actual_string'
              This is to support the use of a string to represent a non-string
              variable such as an enumeration or command:
              E.g. the string might look like "x * 3 + 2" to achieve:
                variable = x * 3 + 2
             )


LIBRARY DEPENDENCY:
  (../src/mc_variable_random_string.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_RANDOM_STRING_HH
#define CML_MONTE_CARLO_VARIABLE_RANDOM_STRING_HH

#include "mc_variable_random_uniform.hh"

#include <random>
#include <string>
#include <vector>

class MonteCarloVariableRandomStringSet : public MonteCarloVariableRandomUniform
{
 public:
  std::vector< std::string> values;
  MonteCarloVariableRandomStringSet( const std::string  & var_name,
                                     unsigned int         seed);

  virtual ~MonteCarloVariableRandomStringSet(){};
  virtual void generate_assignment();
  virtual std::string summarize_variable() const;
  void add_string(std::string);
 private: // and undefined:
  MonteCarloVariableRandomStringSet(const MonteCarloVariableRandomStringSet&);
  MonteCarloVariableRandomStringSet& operator = (
                                     const MonteCarloVariableRandomStringSet&);
};
#endif
