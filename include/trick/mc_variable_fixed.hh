/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class template to support assignment of
           a fixed value to a variable based on its type.)

LIMITATION: (Because these types are typically instantiated
             from the Python input processor via SWIG, use of
             templates is problematic.  Consequently, it would
             require a whole different setup to handle integers
             differently than floats.  Instead, integers and floats
             will both be treated using the "double" data type.
             Values that cannot be assigned to a double (like bool or strings)
             cannot be represented by an instance of this class.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_FIXED_HH
#define CML_MONTE_CARLO_VARIABLE_FIXED_HH

#include "mc_variable.hh"
#include <sstream> // ostringstream

class MonteCarloVariableFixed : public MonteCarloVariable
{
protected:
    int var_type; /* (--) 0: double
                          1: integer
                          2: string;*/
public:
    MonteCarloVariableFixed(const std::string & var_name, double assignment_)
        : MonteCarloVariable(var_name),
          var_type(0)
    {
        type = MonteCarloVariable::Constant;
        assign_double(assignment_);
    }

    MonteCarloVariableFixed(const std::string & var_name, int assignment_)
        : MonteCarloVariable(var_name),
          var_type(1)
    {
        type = MonteCarloVariable::Constant;
        assign_int(assignment_);
    }

    MonteCarloVariableFixed(const std::string & var_name, const std::string & assignment_)
        : MonteCarloVariable(var_name),
          var_type(2)
    {
        include_in_summary = false;
        assignment = assignment_;
        type = MonteCarloVariable::Constant;
        generate_command();
    }

    virtual ~MonteCarloVariableFixed() {}

    void generate_assignment() {} // to make this class instantiable

private: // and undefined:
    MonteCarloVariableFixed(const MonteCarloVariableFixed &);
    MonteCarloVariableFixed & operator=(const MonteCarloVariableFixed &);
};
#endif
