/*******************************TRICK HEADER******************************
PURPOSE: (Provides the front-end interface to the monte-carlo model)

LIBRARY DEPENDENCY:
   ((../src/mc_master.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#ifndef CML_MONTE_CARLO_MASTER_HH
#define CML_MONTE_CARLO_MASTER_HH

#include <list>
#include <string>
#include <utility> // std::pair

#include "mc_python_code.hh"
#include "mc_variable.hh"
#include "mc_variable_file.hh"
#include "mc_variable_fixed.hh"
#include "mc_variable_random.hh"
#include "mc_variable_semi_fixed.hh"

/*****************************************************************************
MonteCarloMaster
Purpose:()
*****************************************************************************/
class MonteCarloMaster
{
public:
    bool active;                 /* (--)
                      The main active flag determining whether an input file should be
                      processed for a monte-carlo run.  This flag is used to manage the
                      configuration of the scenario, including things like which variables
                      to log.*/
    bool generate_dispersions;   /* (--)
          This flag controls whether the variables should be loaded and
          dispersed.  It has no effect if the active flag is off.
          False: Configure the run for MC; this
              configuration typically uses a previously-generated
              monte_input.py file; it does not read in MC variables and
              does not generate new monte_input files.
          True: Use this execution of the sim to read in the MC variables and
              generating the monte-input files.  After doing so, the
              execution will terminate.
              The sim can then be re-run using one of the new monte_input files.
          Default: true.  This is set to false in the monte_input.py files to
           allow the base input file to be processed from within the
           monte-input file without regenerting the monte-input files.*/
    std::string run_name;        /* (--)
               The name of the scenario, used in generating the "MONTE_<run_name>"
               directory, which contains all of the runs.*/
    std::string monte_dir;       /* (--)
              The name of the MONTE<RUN...> directory relative to the SIM directory
              where runs will be generated*/
    std::string input_file_name; /* (--)
        The name of the original file used as the main input file.
        Default: input.py.*/
    bool generate_meta_data;     /* (--)
            Flag indicating whether to generate the meta-data output file.*/
    bool generate_summary;       /* (--)
              Flag indicating whether to generate the dispersion summary file. */
    int minimum_padding;         /* (--)
                The minimum width of the run-number field, e.g. RUN_1 vs RUN_00001;
                The run-number field will be sized to the minimum of this value or the
                width necessary to accommodate the highest number.
                Defaults to 0. */
    size_t monte_run_number;     /* (--)
            A unique identifying number for each run.*/

protected:
    bool input_files_prepared; /* (--)
        Internal flag indicating that the input files have been generated and
        waiting for execution.  Effectively blocks further modifications to
        variables after this flag has been set.*/

    std::string location; /* (--)
        The location in the main sim by which this instance may be addressed
        in an input file. */

    std::list<MonteCarloVariable *> variables; /* (--)
        A STL-list of pointers to instances of all the base-class
        MonteVarVariable instances.  Note that this has to be a list of pointers
        rather than instances because the actual instances are polymorphic;
        making a copy would restrict them to be actual MonteCarloVariable
        instances and we need the polymorphic capabilities for generating the
        monte_input command.*/

    unsigned int num_runs; /* (--)
        The number of runs to execute for this scenario.*/

private:
    std::list<std::pair<std::string, MonteCarloVariableFile *>> file_list; /* (--)
                  A list of filenames being read as part of the MonteVarFile variables
                  being managed by this class. */

public:
    MonteCarloMaster(std::string location);

    virtual ~MonteCarloMaster() {}

    void activate(std::string run_name);
    bool prepare_input_files();
    void add_variable(MonteCarloVariable & variable);
    MonteCarloVariable * find_variable(std::string var_name);
    void remove_variable(std::string var_name);
    void set_num_runs(unsigned int num_runs);
    void execute();
    void collate_meta_data();

private:
    static bool seed_sort(std::pair<unsigned int, std::string> left, std::pair<unsigned int, std::string> right)
    {
        return left.first < right.first;
    }

    // and undefined:
    MonteCarloMaster(const MonteCarloMaster &);
    MonteCarloMaster & operator=(const MonteCarloMaster &);
};
#endif
