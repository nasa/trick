/*******************************TRICK HEADER******************************
PURPOSE: (
   Implementation of a semi-fixed monte-carlo variable.
   The value of a MonteCarloVariableFixed instance is assigned at
   construction time and held at that value for all runs.
   The value of a MonteCarloVariableSemiFixed instance is assigned
   from another MonteCarloVariable generated value for the first
   input file generated, and held at that value for all runs.  So the
   assignment to a Semi-fixed can change each time the input files are
   generated, but it is the same for all input files in any given
   generation.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_SEMI_FIXED_HH
#define CML_MONTE_CARLO_VARIABLE_SEMI_FIXED_HH

#include "mc_variable_random.hh"
#include "trick/message_proto.h"

// TODO Turner 2019/11
//       The reference to a MonteCarloVariable might be difficult to
//       obtain because these are typically created on-the-fly and
//       the handle to the created instance is lost.  Might be
//       nice to provide the seed-variable-name instead of the reference
//       to the seed-variable itself, and have the MonteCarloMaster find
//       the MonteCarloVariable associated with that name.
//       But that's non-trivial and not necessarily desirable, so it is
//       left unimplemented.

class MonteCarloVariableSemiFixed : public MonteCarloVariable
{
protected:
    const MonteCarloVariable & seed_variable; /* (--)
        A reference to another MonteCarloVariable; the value of *this*
        variable is taken from the value of this seed-variable during
        preparation of the first monte-input file. */
    bool command_generated;                   /* (--)
                          flag indicating the fixed command has been generated.*/
public:
    MonteCarloVariableSemiFixed(const std::string & var_name, const MonteCarloVariable & seed_)
        : MonteCarloVariable(var_name),
          seed_variable(seed_),
          command_generated(false)
    {
        type = MonteCarloVariable::Constant;
    }

    virtual ~MonteCarloVariableSemiFixed() {}

    void generate_assignment()
    {
        if(!command_generated)
        {
            // parse the command from seed_variable to get the assignment.
            std::string seed_command = seed_variable.get_command();
            size_t pos_equ = seed_command.find("=");
            if(pos_equ == std::string::npos)
            {
                std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                                      std::string(", Invalid "
                                                  "sequencing\nFor variable ") +
                                      variable_name.c_str() +
                                      std::string(" the necessary pre-dispersion to obtain the\n "
                                                  "random value for assignment has not been completed.\nCannot "
                                                  "generate the assignment for this variable.\n");
                message_publish(MSG_ERROR, message.c_str());
                return;
            }
            assignment = seed_command.substr(pos_equ + 1);
            generate_command();
            insert_units();
            command_generated = true;
        }
    }

private: // and undefined:
    MonteCarloVariableSemiFixed(const MonteCarloVariableSemiFixed &);
    MonteCarloVariableSemiFixed & operator=(const MonteCarloVariableSemiFixed &);
};
#endif
