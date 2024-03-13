/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to support generation and assignment
           of a random value distributed normally.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#include "trick/mc_variable_random_normal.hh"

#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/compareFloatingPoint.hh"
#include <fenv.h>
#include <limits>
#include <algorithm>
#include <assert.h>
#include <sstream> // ostringstream

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariableRandomNormal::MonteCarloVariableRandomNormal(
    const std::string & var_name,
    unsigned int        seed,
    double              mean,
    double              stdev)
  :
  MonteCarloVariableRandom( var_name, seed),
  max_num_tries(10000),
  distribution(mean, stdev),
  min_value(0.0),
  max_value(0.0),
  truncated_low(false),
  truncated_high(false)
{}

/*****************************************************************************
generate_assignment
Purpose:(generates the normally-distributed random number)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::generate_assignment()
{
  double assignment_d = distribution(random_generator);

  if (truncated_low && truncated_high) {
    if (min_value > max_value) {
      std::string message = 
        std::string("File: ") + __FILE__ + ", Line: " +
        std::to_string(__LINE__) + ",  Illegal configuration\nFor variable " +
        variable_name.c_str() + " the specified minimum allowable value (" +
        std::to_string(min_value) + ") >= the specified maximum allowable" +
        + " value (" + std::to_string(max_value) + ").\nOne or both of the" 
        " limits must be changed to generate a random value.\n";
        message_publish(MSG_ERROR, message.c_str());
      exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
    }
    else if (Trick::dbl_is_near( min_value, max_value, 0.0)) {
      std::string message = 
        std::string("File: ") + __FILE__ + ", Line: " +
        std::to_string(__LINE__) + ", Overconstrained configuration\nFor " + 
        "variable" + variable_name.c_str() + "  the specified minimum " + 
        "allowable value and \nthe specified maximum allowable value are "
        "equal (" + std::to_string(min_value) + ").\nThe distribution " +
        " collapses  to a point.\n";
      message_publish(MSG_WARNING, message.c_str());
      assignment_d = min_value;
    }
  }

  size_t num_tries = 0;
  while ( (truncated_high && assignment_d > max_value) ||
          (truncated_low  && assignment_d < min_value)) {
    if ( num_tries < max_num_tries) {
      assignment_d = distribution(random_generator);
      num_tries++;
    }
    else {
      std::string message = 
        std::string("File: ") + __FILE__ + ", Line: " + 
        std::to_string(__LINE__) + " Random value truncation failure\n" + 
        "Could not generate a value for " + variable_name.c_str() +
        "  within the specified domain within\nthe specified maximum " +
        "number of tries (" + 
        std::to_string(max_num_tries) + ").\nAssuming a value equal to:\n" +
        " - midpoint value for a distribution truncated at both ends\n - " +
        "truncation value for a distribution truncated at only one end.\n";
      message_publish(MSG_ERROR, message.c_str());
      // Note - at least one truncation must be defined in order to be in
      // this part of the code.
      if (!truncated_high) { // i.e. truncated-low only
        assignment_d = min_value;
      }
      else if (!truncated_low) { // i.e. truncated-high only
        assignment_d = max_value;
      }
      else { // i.e. truncated both sides
        assignment_d = (max_value + min_value) / 2.0;
      }
      // Stop trying to generate an acceptable value at this point:
      break;
    }
  }
  assign_double(assignment_d);
}

/*****************************************************************************
summarize_variable
Purpose:(Provide a string summarizing the attributes of this MonteCarloVariable)
*****************************************************************************/
std::string MonteCarloVariableRandomNormal::summarize_variable() const
{
  std::ostringstream ss;
  ss << MonteCarloVariable::summarize_variable() << std::string(", dispersion=Normal")
     << ", mean=" << distribution.mean() << ", stddev=" << distribution.stddev()
     << ", min_value=" << min_value << ", max_value=" << max_value
     << ", seed=" << seed_m;
  return (ss.str());
}
/*****************************************************************************
truncate
Purpose:(Truncates the normal distribution to be within +- limit.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate(
     double limit,
     TruncationType truncType)
{
  if (limit < 0)
  {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line " + 
      std::to_string(__LINE__) + ", Out-of-domain error\nNegative " +
      "double-sided truncation specified for variable " + 
      std::to_string(max_num_tries) + "\ntruncate() must receive either " +
      "two limits or one positive limit!\nUsing absolute value of limit." +
      "\nUsing absolute value of limit.\n";
    message_publish(MSG_ERROR, message.c_str());
    limit = -limit;
  }

  if (Trick::dbl_is_near(limit, 0.0, 0.0))
  {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line " + 
      std::to_string(__LINE__) + ",  Configuration error\nZero truncation " +
      "for specified for variable " + variable_name.c_str() + "  which " + 
      "will produce a fixed point\n";
    message_publish(MSG_WARNING, message.c_str());
  }

  // Assign the truncation on both sides:
  truncate_low(-limit, truncType);
  truncate_high(limit, truncType);
}

/***************************************************************************
truncate
Purpose:(Truncates the normal distribution to be within asymmetric limits.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate(
     double min,
     double max,
     TruncationType truncType)
{
  truncate_low(min, truncType);
  truncate_high(max, truncType);
}

/*****************************************************************************
truncate_low
Purpose:(Specifies the lower-bound of the truncation)
Note - min is often -- but not necessarily -- a negative value.
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate_low(
    double min,
    TruncationType truncType)
{
  switch (truncType) {
   case StandardDeviation:
    min_value = distribution.mean() + distribution.stddev() * min;
    break;
   case Relative:
    min_value = distribution.mean() + min;
    break;
   case Absolute:
    min_value = min;
    break;
   // Unreachable code.  All types are covered.
   default:
     std::string message = 
       std::string("File: ") + __FILE__ + ", Line " +
       std::to_string(__LINE__) + ", Invalid TruncationType\nInvalid " +
       "truncation type passed to truncate_low for variable " + 
       variable_name.c_str() + ".\nMinimum will not be applied.\n";
     message_publish(MSG_ERROR, message.c_str());
    return;
  }
  truncated_low = true;
}
/*****************************************************************************
truncate_high
Purpose:(Specifies the upper-bound of the truncation)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate_high( double max,
                                               TruncationType truncType)
{
  switch (truncType) {
   case StandardDeviation:
    max_value = distribution.mean() + distribution.stddev() * max;
    break;
   case Relative:
    max_value = distribution.mean() + max;
    break;
   case Absolute:
    max_value = max;
    break;
   // Unreachable code.  All types are covered.
   default:
     std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " + 
      std::to_string(__LINE__) + ", Invalid TruncationType\nInvalid " +
      "truncation type passed to truncate_high for variable " + 
      variable_name.c_str() + ".\nMaximum will not be applied.\n";
      message_publish(MSG_ERROR, message.c_str());
    return;
  }
  truncated_high = true;
}

/*****************************************************************************
untruncate
Purpose:(Remove truncation flags.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::untruncate()
{
  truncated_low = false;
  truncated_high = false;
}

