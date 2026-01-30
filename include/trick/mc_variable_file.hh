/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a file-lookup  assignment

LIBRARY DEPENDENCY:
  (../src/mc_variable_file.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_VARIABLE_FILE_HH
#define CML_MONTE_CARLO_VARIABLE_FILE_HH

#include <fstream> // ifstream
#include <list>
#include <random> // default_random_engine, uniform_int_distribution

#include "trick/mc_variable.hh"

/*****************************************************************************
MonteCarloVariableFile
Purpose:(
    Grabs a value from a data file.
    The data value is located within the file at some column number and
    row number.
    The column number is specified for the variable.
    The row -- or line -- number changes from run to run.
    The file is expected to provide consistent presentation of data from
    row to row.)
*****************************************************************************/
class MonteCarloVariableFile : public MonteCarloVariable
{
public:
    size_t max_skip;   /* (--)
        The maximum number of lines to skip in a file.  This defaults to 0,
        indicating that the file will be read sequentially.*/
    bool is_dependent; /* (--)
    A flag indicating that this instance is dependent upon another instance
    for reading the file "filename".  Only one instance will read each
    filename.  Default: false. */
protected:
    std::mt19937 rand_gen;                          /* (--)
                                A random number generator used in the case that the lines are not read
                                sequentially.  This will generate a value from 0 to max_skip indicating
                                how many lines should be skipped.*/
    std::string filename;                           /* (--)
                                 The name of the file containing the data for this variable.  Assigned
                                 at construction.*/
    size_t column_number;                           /* (--)
                                  The column number indicating from where in the data file the data for
                                  this variable should be extracted.*/
    size_t first_column_number;                     /* (--)
                            Usually used to distinguish between whether the first column should be
                            identified with a 0 or 1, but extensible to other integers as well.
                            Default: 1. */
    std::list<MonteCarloVariableFile *> dependents; /* (--)
       A list of MonteVarVariableFile instances that use the same file as
       this one.  This list is only populated if this instance was the first
       registered to use this file.*/
    std::ifstream file;                             /* (--)
                                    Input file stream being the file containing the data.*/

public:
    MonteCarloVariableFile(const std::string & var_name,
                           const std::string & filename,
                           size_t column_number_,
                           size_t first_column_number = 1);

    virtual ~MonteCarloVariableFile() {}

    void initialize_file();
    void generate_assignment();
    virtual std::string summarize_variable() const;

    void register_dependent(MonteCarloVariableFile *);

    virtual void shutdown()
    {
        file.close();
    }

    bool has_dependents()
    {
        return (dependents.size() > 1);
    }

    size_t get_column_number()
    {
        return column_number;
    }

    size_t get_first_column_number()
    {
        return first_column_number;
    }

    const std::string & get_filename()
    {
        return filename;
    }

    const std::list<MonteCarloVariableFile *> & get_dependents()
    {
        return dependents;
    }

protected:
    void process_line();
    static bool sort_by_col_num(MonteCarloVariableFile *, MonteCarloVariableFile *);

private: // and undefined:
    MonteCarloVariableFile(const MonteCarloVariableFile &);
    MonteCarloVariableFile & operator=(const MonteCarloVariableFile &);
};
#endif
