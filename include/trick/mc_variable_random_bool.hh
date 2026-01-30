/*******************************TRICK HEADER******************************
PURPOSE: ( Uses the RandomString generator to generate either a "True" or
           "False" string for assignment through the SWIG interface.
           Note that SWIG uses the Python uppercase True/False rather than
           C++ lowercase true/false identifiers.

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_RANDOM_BOOL_HH
#define CML_MONTE_CARLO_VARIABLE_RANDOM_BOOL_HH

#include "mc_variable_random_string.hh"

/*****************************************************************************
MonteCarloVariableRandomBool
Purpose:(Generates either a True or False string for assignment)
*****************************************************************************/
class MonteCarloVariableRandomBool : public MonteCarloVariableRandomStringSet
{
public:
    MonteCarloVariableRandomBool(const std::string & var_name, unsigned int seed)
        : MonteCarloVariableRandomStringSet(var_name, seed)
    {
        add_string("False");
        add_string("True");
        include_in_summary = true; // String variables are excluded by default
                                   // because they may contain commas, which would
                                   // cause trouble in the comma-delimited summary
                                   // file. However, this is a special case in which
                                   // the possible strings are "True" and "False".
    }

    virtual ~MonteCarloVariableRandomBool() {}

private: // and undefined:
    MonteCarloVariableRandomBool(const MonteCarloVariableRandomBool &);
    MonteCarloVariableRandomBool & operator=(const MonteCarloVariableRandomBool &);
};
#endif
