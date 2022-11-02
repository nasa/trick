/*******************************TRICK HEADER******************************
PURPOSE: ( Base class for the MonteCarloVariable type)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/

#include "trick/mc_variable.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/exec_proto.h"
#include <sstream> // ostringstream

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariable::MonteCarloVariable(
    const std::string & var_name)
  :
  units(),
  include_in_summary(true),
  variable_name(var_name),
  assignment(),
  command(),
  type(Undefined)
{}

/*****************************************************************************
insert_units
Purpose:(Provides a unit-conversion insertion into an established command)
*****************************************************************************/
void
MonteCarloVariable::insert_units()
{
  // If no units specified, nothing to do here.
  if (units.empty()) {
    return;
  }

  // Unreachable code in current implementation.  insert_units is only ever
  // called immediately after generate_command, which cannot possibly generate
  // an empty string.
  if (command.empty()) {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " + 
      std::to_string(__LINE__) + ", Sequencing error\nVariable " + 
      variable_name.c_str() + "  has units specified (" + units.c_str() + 
      ") but no command generated.\nThe command must be generated before " + 
      "applying units.Will attempt to generate the command to "
      "avoid terminal fault but this\nmay not be what was intended.\n";
    message_publish(MSG_ERROR, message.c_str());
  }
  // parse the command
  size_t pos_equ = command.find("=");
  if (pos_equ == std::string::npos) {
      // Unreachable code in current implementation.  insert_units is only ever
      // called immediately after generate_command, and even if all else fails,
      // generate_command produces a command with an = symbol in it, So an =
      // will always be found.
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " + 
      std::to_string(__LINE__) + " Invalid command\nFor variable " + 
      variable_name.c_str() + ", the command is poorly formed.\nCannot " +
      "apply units to this command.\n";
    message_publish(MSG_ERROR, message.c_str());
    exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
    return;
  }

  // TODO: Pick a unit-conversion mechanism
  //       Right now, the only one available is Trick:
  trick_units( pos_equ+1);
}

/*****************************************************************************
trick_units
Purpose:(inserts the Trick unit conversion mechanism into an established
         command string.)
*****************************************************************************/
void
MonteCarloVariable::trick_units(
    size_t insertion_pt)
{
  command.insert(insertion_pt, " trick.attach_units(\"" + units + "\",");
  command.append(")");
}

/*****************************************************************************
generate_assignment_internal
Purpose:()
*****************************************************************************/
void
MonteCarloVariable::assign_double(
  double value)
{
  std::ostringstream ostring;
  ostring.precision(16);
  ostring << value;
  assignment = ostring.str();
  generate_command();
  insert_units();
}
/****************************************************************************/
void
MonteCarloVariable::assign_int(
  int value)
{
  assignment = std::to_string(value);
  generate_command();
  insert_units();
}
/****************************************************************************/
void
MonteCarloVariable::generate_command()
{
  command = "\n" + variable_name + " = " + assignment;
}
