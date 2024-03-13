/*******************************TRICK HEADER******************************
PURPOSE: ( Base class for the MonteCarloVariable type)

LIBRARY DEPENDENCY:
   ((../src/mc_variable.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_HH
#define CML_MONTE_CARLO_VARIABLE_HH

#include <string>

class MonteCarloVariable
{
 public:
  enum MonteCarloVariableType
  {
    Undefined = 0,
    Calculated,
    Constant,
    Execute,
    Prescribed,
    Random
  };

  std::string units; /* (--)
      optional setting in the case where the specified values are in units
      different from the native units of the variable.
      These are the units associated with the specified-value.*/
  bool include_in_summary; /* (--)
      Flag telling MonteCarloMaster whether to include this variable in the
      dispersion summary file. The default depends on the type of variable but
      is generally true. */

 protected:
  std::string variable_name; /* (--)
      The name of the sim-variable being assigned by this instance. */
  std::string assignment; /* (--)
      The value assigned to the variable. Used in MonteCarloMaster to generate
      the dispersion summary file. */
  std::string command; /* (--)
      the command that gets pushed to the monte_input input file.*/
  MonteCarloVariableType type; /* (--)
      Broad categorization of types of MonteCarloVariable.  This is set in the
      constructor of the specific classes derived from MonteCarloVariable and
      provides information to the MonteCarloMaster about what general type of
      variable it is dealing with.*/


 public:
  MonteCarloVariable( const std::string & var_name);
  virtual ~MonteCarloVariable() {};


  virtual void generate_assignment() = 0;
  virtual void shutdown() {}; // deliberately empty

  // These getters are intended to be used by the MonteCarloMaster class in
  // preparing the input files and summary data files.  They may also be used
  // in the user interface, but -- especially get_assignment and get_command --
  // have limited use there.
  const std::string & get_command() const {return command;}
  const std::string & get_variable_name() const {return variable_name;}
  const std::string & get_assignment() const {return assignment;}
  virtual MonteCarloVariableType get_type() const {return type;}
  virtual std::string  get_type_str() const;
  virtual unsigned int get_seed() const {return 0;}
  virtual std::string summarize_variable() const;

 protected:
  void insert_units();
  void trick_units(size_t);
  void assign_double(double value);
  void assign_int(int value);
  void generate_command();

 private: // and undefined:
  MonteCarloVariable( const MonteCarloVariable &);
  MonteCarloVariable& operator = (const MonteCarloVariable&);
};

#endif
