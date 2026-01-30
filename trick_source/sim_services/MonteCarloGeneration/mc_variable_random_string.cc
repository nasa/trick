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
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include <sstream> // ostringstream

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariableRandomStringSet::MonteCarloVariableRandomStringSet(const std::string & var_name, unsigned int seed)
    : MonteCarloVariableRandomUniform(var_name, seed, 0, 1),
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
void MonteCarloVariableRandomStringSet::generate_assignment()
{
    // Protect against accessing 0th index of an empty list. In this case,
    // generate_command() is never called
    if(values.size() > 0)
    {
        // generate a random number on the interval [0,1)
        double pre_ix = distribution(random_generator);
        // convert to an integer between 0 and max-index of the "values" vector.
        size_t ix = static_cast<size_t>(pre_ix * values.size());
        // send the string at that index to the command-generation.
        // TODO: This fales if values.size() is zero!!!
        assignment = values[ix];
        generate_command();
    }
    else
    {
        std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                              ", No values for MonteCarloVariableRandomStringSet\n"
                              "Length of values vector is zero for variable: " +
                              variable_name + ", Did " + "you forget to call add_string()?\n";
        message_publish(MSG_WARNING, message.c_str());
    }
}

/*****************************************************************************
summarize_variable
Purpose:(Provide a string summarizing the attributes of this MonteCarloVariable)
*****************************************************************************/
std::string MonteCarloVariableRandomStringSet::summarize_variable() const
{
    std::ostringstream ss;
    // TODO: Here we create a string in [val1, val2, ... ] form, but this is
    // ambigious because if there's a comma in any value, parsing the resultant
    // string from the MonteCarlo_Meta_data_output file could fail. What delimter
    // or approach would be better? -Jordan 10/2023
    ss << MonteCarloVariableRandomUniform::summarize_variable() << ", values=[";
    if(values.size() > 0)
    {
        for(auto val_it : values)
        {
            ss << val_it << ",";
        }
    }
    ss << "]";
    return (ss.str());
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
void MonteCarloVariableRandomStringSet::add_string(std::string new_string)
{
    values.push_back(new_string);
}
