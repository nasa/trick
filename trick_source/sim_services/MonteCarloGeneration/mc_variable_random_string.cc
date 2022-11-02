/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to randomly pick one of a set of
           character strings.  These strings could represent actual string
           variables, or enumerated types, or commands, or any other concept
           that can be expressed as an assignment.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#include "trick/mc_variable_random_string.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariableRandomStringSet::MonteCarloVariableRandomStringSet(
    const std::string         & var_name,
    unsigned int                seed)
  :
  MonteCarloVariableRandomUniform( var_name, seed, 0, 1),
  values()
{
  include_in_summary = false; // The summary file is comma-delimited. String
                              // variables are excluded by default because they
                              // may contain commas.
}

/*****************************************************************************
generate_assignment
Purpose:(pick one string at random)
*****************************************************************************/
void
MonteCarloVariableRandomStringSet::generate_assignment()
{
  // generate a random number on the interval [0,1)
  double pre_ix = distribution(random_generator);
  // convert to an integer between 0 and max-index of the "values" vector.
  size_t ix = static_cast<size_t> (pre_ix * values.size());
  // send the string at that index to the command-generation.
  assignment = values[ix];
  generate_command();
}
/*****************************************************************************
add_string
Purpose:(Adds an option to the set of strings)

NOTE - When adding a string realize that the content of the selected string
       will be assigned as written.  Consequently, if the value is intended for
       assignment to an actual string or char variable, the contents of the
       string being added here should include the quotes.
       E.g. the string might look like: "'actual_string'" so that the assignment
            would look like:
                variable = 'actual_string'
            or "\"actual_string\""
                variable = "actual_string"
       This is to support the use of a string to represent a non-string variable
       such as an enumeration or command:
       E.g. adding a string here that contains the characters "x * 3 + 2" can
            be used to achieve an assignment like:
                variable = x * 3 + 2
*****************************************************************************/
void
MonteCarloVariableRandomStringSet::add_string( std::string new_string)
{
  values.push_back(new_string);
}
