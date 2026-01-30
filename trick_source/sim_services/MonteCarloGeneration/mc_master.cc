/*******************************TRICK HEADER******************************
PURPOSE: (Provides the front-end interface to the monte-carlo model)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/
#include "trick/mc_master.hh"

#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include <cstdlib>  // system
#include <fstream>  // std::ofstream
#include <iterator> // std::prev

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloMaster::MonteCarloMaster(std::string location_)
    : active(false),
      generate_dispersions(true),
      run_name(),
      monte_dir(),
      input_file_name("input.py"),
      generate_meta_data(false),
      generate_summary(true),
      minimum_padding(0),
      monte_run_number(0),
      input_files_prepared(false),
      location(location_),
      variables(),
      num_runs(0)
{
}

/*****************************************************************************
activate
Purpose:()
*****************************************************************************/
void MonteCarloMaster::activate(std::string run_name_)
{
    run_name = run_name_;
    monte_dir = "MONTE_" + run_name;
    active = true;
}

/*****************************************************************************
prepare_input_files
Purpose:(Creates the top-level MONTE_<run_name> directory, clearing out any
           existing content.
         Creates the RUN_<run_num> subdirectories
         Creates the monte_<input.py> in each RUN_<run_num> subdirectory.)
*****************************************************************************/
bool MonteCarloMaster::prepare_input_files()
{
    if(input_files_prepared)
    {
        std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                              ", Invalid sequence\n" +
                              "Attempted to "
                              "generate a set of input files, but this action has\nalready been "
                              "completed. Keeping the original set of input files.\nIgnoring "
                              "the later instruction.\n";
        message_publish(MSG_ERROR, message.c_str());
        return true;
    }

    // Create the new MONTE_ dir name where runs will go
    std::string command = "mkdir -p " + monte_dir;
    system(command.c_str());
    command = "rm -rf " + monte_dir + "/RUN_*";
    // TODO should we check return code for failure here? -Jordan 2/2020
    system(command.c_str());

    // Based on the number of runs, generate an appropriately-sized string to
    // contain the run number.
    // E.g. For 1-10 runs, need only 1 numeric character to supply:
    //         RUN_0, RUN_1, RUN_2, ..., RUN_9
    //      For between 10001-100000 runs, need 5 numeric characters to supply:
    //         RUN_00000, RUN_00001, ..., RUN_99999
    // This string length can be set to a MINIMUM value with the variable
    // minimum_padding.
    int max_length = std::to_string(num_runs - 1).size();
    if(max_length < minimum_padding)
    {
        max_length = minimum_padding;
    }
    std::string run_num_base(max_length, '0');

    // create the master list of varaibles being recorded in the monte_values
    // files.  note that this is not all the variables, only those whose values
    // are being recorded ina s eparate file for every run.  For a full list of
    // variables and their type, see the meta-data file.
    if(generate_summary)
    {
        // Create the summary file and variable list
        std::string filename = monte_dir + "/monte_variables";
        std::ofstream variable_list(filename);
        // Check for success of file creation
        if(!variable_list.is_open())
        {
            std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                                  ", I/O error\nUnable to open the " +
                                  "variable summary files for writing.\nDispersion summary will not " +
                                  "be generated.\n";
            message_publish(MSG_ERROR, message.c_str());
            generate_summary = false;
        }
        else
        {
            // Write the variable list
            variable_list << "run_number\n";
            for(auto var_it : variables)
            {
                if(var_it->include_in_summary)
                {
                    variable_list << var_it->get_variable_name() << ", " << var_it->units << "\n";
                }
            }
            variable_list.close();
        }
    }

    // Process each input file one at a time, and write all variables into each
    // file before moving on to the next file. This is better than trying to
    // keep a large number of files open so each variable can be written into
    // all files before moving on to the next variable.
    for(unsigned int run_num = 0; run_num < num_runs; ++run_num)
    {
        std::string run_num_str(run_num_base);
        std::string run_num_str_partial = std::to_string(run_num);
        int length = run_num_str_partial.size();

        // I'm going to be replacing contents of the string of zeros with the
        // run-number; make sure the run-number doesn't contain more characters
        // than in the string of zeros.  Because the zeros-string is as long as
        // the largest number, this should always pass.
        // Unreachable code in current implementation.  run_num_str is sized to
        // accommodate run_num_base, which ahs been given as many zeroes as the
        // number of characters in the largest run number.
        if(run_num_str_partial.size() > run_num_str.size())
        {
            std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                                  ", Sizing Error\nAttempted to create a " +
                                  "filename with a run-number that exceeds the\npre-generated size " +
                                  "(e.g. trying to fit the number 10000 into 4 characters.\nThis " +
                                  "should never happen.\n";
            message_publish(MSG_ERROR, message.c_str());
            exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
        }
        // else
        run_num_str.replace(max_length - length, length, run_num_str_partial);

        // Create the directories.
        command = "mkdir -p " + monte_dir + "/RUN_" + run_num_str;
        system(command.c_str());

        // Next write the input file into the created directory
        std::string filename_root = monte_dir + "/RUN_" + run_num_str + "/monte_";
        std::string filename = filename_root + input_file_name;
        std::ofstream input_file(filename);
        // Check for success of file-open using ofstream's failbit.
        if(input_file.fail())
        {
            std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                                  ", I/O error\nUnable to open file " + filename.c_str() + " for writing.";
            message_publish(MSG_ERROR, message.c_str());
            exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
        }

        // print the default (common) content to the top of the file and add the
        // run-number identification for any variables that may depend on this.
        input_file << location
                   << ".active = True"
                      "\n"
                   << location
                   << ".generate_dispersions = False\n"
                      "\nexec(open('"
                   << run_name << "/" << input_file_name
                   << "').read())"
                      "\n"
                   << location << ".monte_run_number = " << run_num << "\n";

        // Process all monte-carlo variables for this run.  Note -- each variable
        // has its own random generator, so the insertion of other variables
        // cannot interfere with the random number generation sequence.
        for(auto var_it : variables)
        {
            var_it->generate_assignment();
            input_file << var_it->get_command();
        }
        input_file << "\n";
        input_file.close();

        // Generate the run-level dispersions list
        if(generate_summary)
        {
            std::ofstream disp_list(filename_root + "values");
            if(disp_list.is_open())
            {
                disp_list << run_num_str;
                // Print the run number at the beginning of the line
                for(auto var_it : variables)
                {
                    if(var_it->include_in_summary)
                    {
                        disp_list << ", " << var_it->get_assignment();
                    }
                }
                disp_list << "\n";
                disp_list.close();
            }
            // Unreachable code in current implementation.
            else
            {
                std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                                      ", Output failure\nFailed to record " + "summary data for run " +
                                      run_num_str.c_str() + ".\n";
                message_publish(MSG_ERROR, message.c_str());
                generate_summary = false;
            }
        }
    }

    // Consolidate all individual run monte_values files.
    if(generate_summary)
    {
        std::string all_runs_filename = monte_dir + "/monte_values_all_runs";
        command = "rm -f " + all_runs_filename;
        system(command.c_str());
        command = "cat " + monte_dir + "/RUN_*/monte_values >> " + all_runs_filename;
        system(command.c_str());
        command = "for r in " + monte_dir + "/RUN_*;" + "do ln -s ../monte_variables $r/monte_variables; done";
        system(command.c_str());
    }

    input_files_prepared = true;
    return true;
}

/*****************************************************************************
add_variable
Purpose:(Adds a pointer to an instantiated MonteCarloVariable to the
         master-list.  This master-list will be processed in generating the
         random assignments that are recorded in the monte_input.py files.)
*****************************************************************************/
void MonteCarloMaster::add_variable(MonteCarloVariable & variable)
{
    if(input_files_prepared)
    {
        std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                              ", Invalid sequence\nAttempted to add a " + "new variable " +
                              variable.get_variable_name().c_str() + " to run " + run_name.c_str() +
                              ", but the input files have already been " +
                              "generated.\nCannot modify input files to accommodate this new " +
                              "variable.\nAddition of variable rejected.\n";
        message_publish(MSG_ERROR, message.c_str());
        return;
    }

    // check for uniqueness
    for(auto var_it = variables.begin(); var_it != variables.end(); ++var_it)
    {
        if((**var_it).get_variable_name() == variable.get_variable_name())
        {
            std::string message = std::string("File: ") + __FILE__ + ", Line:  " + std::to_string(__LINE__) +
                                  ",  Duplicated variable.\nAttempted " + "to add two settings for variable " +
                                  variable.get_variable_name().c_str() +
                                  ".\nTerminating to allow resolution of which setting to use.\n";
            message_publish(MSG_ERROR, message.c_str());
            exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
        }
    }

    // if the variable is of type MonteCarloVariableFile, check for
    // other MonteCarloVariableFiles that use the same file.
    MonteCarloVariableFile * file_variable = dynamic_cast<MonteCarloVariableFile *>(&variable);
    if(file_variable != NULL)
    {
        std::string filename = file_variable->get_filename();
        bool found_file = false;
        for(auto it = file_list.begin(); it != file_list.end(); ++it)
        {
            if(filename == (*it).first)
            {
                (*it).second->register_dependent(file_variable);
                found_file = true;
                break;
            }
        }
        if(!found_file)
        {
            std::pair<std::string, MonteCarloVariableFile *> new_pair(filename, file_variable);
            file_list.push_back(new_pair);
        }
    }

    // Finally, add this variable to the list
    variables.push_back(&variable);
}

/*****************************************************************************
find_variable
Purpose:
   Get a pointer to a MonteCarloVariable instance based on its "name" -- i.e.
   the variable for which it is generating a value.
Limitations:
    - Only returns the base-class pointer, so this has limited
      versatility.  If the desire is to modify the distribution parameters
      or other characteristic of a MCVariable after it has been
      generated, the returned pointer may require an additional
      dynamic-cast to make the characteristics of the distribution type
      available.
    - Return value must be checked for NULL
*****************************************************************************/
MonteCarloVariable * MonteCarloMaster::find_variable(std::string var_name)
{
    for(auto it : variables)
    {
        if(var_name == it->get_variable_name())
        {
            return it;
        }
    }
    std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                          ", Invalid name\nCould not find MonteCarlo " + "variable with name " + var_name.c_str() +
                          ".\nReturning a NULL pointer.\n";
    message_publish(MSG_ERROR, message.c_str());
    return NULL;
}

/*****************************************************************************
remove_variable
Purpose:(remove a variable from distribution after it has been added)
Limitation:(Must be run before "execute"; once the files have been generated, it is too late to remove a variable)
*****************************************************************************/
void MonteCarloMaster::remove_variable(std::string var_name)
{
    // NOTE - cannot use find_variable(...); that returns a pointer to the
    // variable and this method needs the list iterator addressing the variable.
    for(auto it = variables.begin(); it != variables.end(); ++it)
    {
        if(var_name == (*it)->get_variable_name())
        {
            variables.erase(it);
            return;
        }
    }
    std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                          ", Invalid name\nAttempt to remove " + "MonteCarlo variable with name " + var_name.c_str() +
                          " FAILED.\nDid " + "not find a variable with that name.\n";
    message_publish(MSG_WARNING, message.c_str());
}

/*****************************************************************************
set_num_runs
Purpose:(Sets the intended number of runs for this scenario)
*****************************************************************************/
void MonteCarloMaster::set_num_runs(unsigned int num_runs_)
{
    if(input_files_prepared)
    {
        std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                              ", Invalid sequence\nAttempted to set the " + "number of runs to " +
                              std::to_string(num_runs_) + ", but the " + "input files have\nalready been generated.";
        message_publish(MSG_ERROR, message.c_str());
    }
    else
    {
        num_runs = num_runs_;
    }
}

/*****************************************************************************
execute
Purpose:(The main executive.  This should be run as an initialization-class
         job very early in the initialization cycle, after all the input
         processor operations have completed.)
*****************************************************************************/
void MonteCarloMaster::execute()
{
    if(!active || !generate_dispersions)
    {
        return;
    }

    for(auto it = file_list.begin(); it != file_list.end(); ++it)
    {
        (*it).second->initialize_file();
    }

    prepare_input_files();

    if(generate_meta_data)
    {
        collate_meta_data();
    }

    // clean up any lingering aspects of the MonteCarloVariable instances.
    std::list<MonteCarloVariable *>::iterator var_it = variables.begin();
    for(; var_it != variables.end(); ++var_it)
    {
        (**var_it).shutdown();
    }

    std::string message = "\nMonte-Input files generated in " + monte_dir +
                          "\nRuns can now be launched using the generated monte-input " +
                          "files and\nmanaged with an external load-management system, " + "such as SLURM.\n" +
                          "This simulation is complete.  Exiting.\n";
    exec_terminate_with_return(0, __FILE__, __LINE__, message.c_str());
}

/*****************************************************************************
collate_meta_data
Purpose:(Generates an output of metadata describing the variables, their
         types, and other identifying characteristics)
*****************************************************************************/
void MonteCarloMaster::collate_meta_data()
{
    std::string filename = "MonteCarlo_Meta_data_output";
    if(input_files_prepared)
    { // so directory exists
        filename = monte_dir + "/" + filename;
    }

    std::ofstream meta_data(filename);
    // Check for success of file-open.
    if(meta_data.fail())
    {
        std::string message = std::string("File: ") + __FILE__ + ", Line: " + std::to_string(__LINE__) +
                              ", I/O error\nUnable to open file " + filename.c_str() +
                              " for writing.\nAborting generation of meta-data.\n";
        message_publish(MSG_WARNING, message.c_str());
        return;
    }

    if(!input_files_prepared)
    {
        meta_data << "Generating meta-data on the current configuration.\n"
                  << "The input files have not yet been generated which means this\n"
                  << "configuration has not been finalized and is subject to change.\n"
                  << "Sending meta-data to top-level directory.\n";
    }

    // Capture and alphabetize all variable names with their respective variable
    // type; count the number of each type.
    std::list<std::string> variable_details;
    std::list<std::string> exec_file_names;
    std::list<std::pair<unsigned int, std::string>> random_variables;
    unsigned int count_calc = 0;
    unsigned int count_const = 0;
    unsigned int count_exec = 0;
    unsigned int count_presc = 0;
    unsigned int count_rand = 0;
    unsigned int count_undef = 0;

    for(const auto * var_it : variables)
    {
        switch(var_it->get_type())
        {
            // Unreachable case in current implementation.
            // All current variable classes have been given a "type"
            default:
                variable_details.push_back(var_it->summarize_variable());
                count_undef++;
                break;
            case MonteCarloVariable::Calculated:
                variable_details.push_back(var_it->summarize_variable());
                count_calc++;
                break;
            case MonteCarloVariable::Constant:
                variable_details.push_back(var_it->summarize_variable());
                count_const++;
                break;
            case MonteCarloVariable::Execute:
                exec_file_names.push_back(var_it->summarize_variable());
                count_exec++;
                break;
            case MonteCarloVariable::Prescribed:
                variable_details.push_back(var_it->summarize_variable());
                count_presc++;
                break;
            case MonteCarloVariable::Random:
                variable_details.push_back(var_it->summarize_variable());
                count_rand++;
                std::pair<unsigned int, std::string> var(var_it->get_seed(), var_it->get_variable_name());
                random_variables.push_back(var);
                break;
        }
    }
    variable_details.sort();

    meta_data << "\n\n*************************** SUMMARY **************************\n"
              << variables.size() << " total assignments\n  -  " << count_const << " constant values\n  -  "
              << count_calc << " calculated variables\n  -  " << count_presc
              << " prescribed (file-based) variables\n  -  " << count_rand << " random variables\n  -  " << count_exec
              << " files for execution\n  -  " << count_undef << " variables of undefined type"
              << "\n\n********************* LIST OF VARIABLES, TYPES****************\n";

    std::list<std::string>::iterator var_name_it = variable_details.begin();
    for(; var_name_it != variable_details.end(); ++var_name_it)
    {
        meta_data << (*var_name_it) << "\n";
    }
    meta_data << "**************************************************************\n";

    if(!exec_file_names.empty())
    {
        meta_data << "\n\n*********** LIST OF EXECUTABLE FILES AND FUNCTIONS ***********\n";
        for(const auto & var_name_it_temp : exec_file_names)
        {
            meta_data << var_name_it_temp << "\n***\n";
        }
        meta_data << "**************************************************************\n";
    }

    if(!file_list.empty())
    {
        meta_data << "\n\n***** LIST OF DATA FILES AND THE VARIABLES THEY POPULATE *****";
        for(const auto & file_it : file_list)
        {
            meta_data << "\n******\n" << file_it.first << "\n";
            const std::list<MonteCarloVariableFile *> & dependents = file_it.second->get_dependents();
            for(const auto & itv : dependents)
            {
                meta_data << itv->get_column_number() << "  " << itv->get_variable_name() << "\n";
            }
        }
        meta_data << "**************************************************************\n";
    }

    // Need to check the seeds on the random variables for inadvertent
    // correlation between variables.  If there are no random variables, there is
    // nothing to do.
    if(!random_variables.empty())
    {
        random_variables.sort(seed_sort);

        meta_data << "\n\n*****Duplicate seeds; check for intentional correlations*****\n";
        std::list<std::pair<unsigned int, std::string>>::iterator rand_it = random_variables.begin();
        unsigned int prev_seed = (*rand_it).first;
        // start checking at the second element ... obviously the first element
        // doesn't match its previous value, and starting with the second element
        // guarantees that there will be a previous element.
        ++rand_it;
        bool in_duplicate = false;
        for(; rand_it != random_variables.end(); ++rand_it)
        {
            // if this seed matches the value of the previous seed, need to
            // record it and the variable associated with it.
            // Also record the variable associated with the previous list entry (i.e.
            // the one with the same seed), but need to be careful here in the case
            // of having 3 (or more) variables with matching seeds to avoid
            // multiple records being made of the middle duplicates.
            if((*rand_it).first == prev_seed)
            {
                if(!in_duplicate)
                {
                    // if this is the first match for a given seed, record the previous
                    // entry as well.
                    meta_data << (*std::prev(rand_it)).first << "     " << (*std::prev(rand_it)).second << "\n";
                    in_duplicate = true;
                }
                // Then record this entry for all matches.
                meta_data << (*rand_it).first << "     " << (*rand_it).second << "\n";
            }
            else
            {
                in_duplicate = false;
                prev_seed = (*rand_it).first;
            }
        }
        meta_data << "**************************************************************\n";
        meta_data << "\n\n************************  ALL SEEDS *************************\n";
        for(rand_it = random_variables.begin(); rand_it != random_variables.end(); ++rand_it)
        {
            meta_data << (*rand_it).first << "     " << (*rand_it).second << "\n";
        }
    }
    meta_data.close();
}
