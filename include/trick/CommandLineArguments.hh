/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.))
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef COMMANDLINEARGUMENTS_HH
#define COMMANDLINEARGUMENTS_HH

#include <string>

namespace Trick {

    class CommandLineArguments {

        public:

            int argc ;                       /* ** argc */
            char ** argv ;                   /* ** argv */

            bool output_dir_timestamped ;    /* -- record data into a separate timestamped directory */

            std::string default_dir ;             /* -- Directory of executable */
            std::string cmdline_name ;            /* -- Name of sim from the cmdline */
            std::string run_dir ;                 /* -- Run directory name "RUN_<unique_tag>" */
            std::string user_output_dir ;         /* -- user specified output dir */
            std::string time_stamp_dir ;          /* -- time stamp sub directory */
            std::string output_dir ;              /* -- Directory name to hols S_job_execution, chkpnt_*
                                                   files, and the rt logging files */
            std::string input_file ;              /* -- Simulation input data file name */

            CommandLineArguments() ;

            /**
             @brief Copies the command line arguments and saves them for all of the other Trick
             core classes to use.
             @param narg - the number of arguments
             @param args - the list of argument strings.
             @return always 0
            */
            int process_sim_args(int narg , char ** args ) ;

            /**
             @brief Command to get the number of command line arguments
             @return int number of arguments
            */
            int get_argc() ;

            /**
             @brief Command to get the command line arguments
             @return char ** pointing to the argument list
            */
            char ** get_argv() ;

            /**
             @brief @userdesc Command to get the output directory.
             @par Python Usage:
             @code <my_output_dir> = trick.command_line_args_get_output_dir() @endcode
             @return string output_dir
            */
            std::string get_output_dir() ;

            /* reference return version used with C bound API functions.  Should not be used by users */
            std::string & get_output_dir_ref() ;

            /**
             @brief @userdesc Command to get the user_output directory.
             @par Python Usage:
             @code <my_user_output_dir> = trick.command_line_args_get_user_output_dir() @endcode
             @return string user_output_dir
            */
            std::string get_user_output_dir() ;

            /* reference return version used with C bound API functions.  Should not be used by users */
            std::string & get_user_output_dir_ref() ;

            /**
             @brief @userdesc Command to get the input file.
             @par Python Usage:
             @code <my_input_file> = trick.command_line_args_get_input_file() @endcode
             @return string input_file
            */
            std::string get_input_file() ;

            /* reference return version used with C bound API functions.  Should not be used by users */
            std::string & get_input_file_ref() ;

            /**
             @brief @userdesc Command to get the default directory.
             @par Python Usage:
             @code <my_default_dir> = trick.command_line_args_get_default_dir() @endcode
             @return string default_dir
            */
            std::string get_default_dir() ;

            /* reference return version used with C bound API functions.  Should not be used by users */
            std::string & get_default_dir_ref() ;

            /**
             @brief @userdesc Command to get the command line name of the simulation
             @par Python Usage:
             @code <my_cmdline_name> = trick.command_line_args_get_cmdline_name() @endcode
             @return string cmdline_name
            */
            std::string get_cmdline_name() ;

            /* reference return version used with C bound API functions.  Should not be used by users */
            std::string & get_cmdline_name_ref() ;

            /**
             @brief @userdesc Command to turn the timestamped output directory on
             @par Python Usage:
             @code trick.output_dir_timestamped_on() @endcode
             @return always 0
            */
            int output_dir_timestamped_on() ;

            /**
             @brief @userdesc Command to turn the timestamped output directory off
             @par Python Usage:
             @code trick.output_dir_timestamped_off() @endcode
             @return always 0
            */
            int output_dir_timestamped_off() ;

            /**
             @brief @userdesc Command to set the output_dir value to something other than default
             @par Python Usage:
             @code trick.set_output_dir("<directoryname>") @endcode
            */
            void set_output_dir(std::string output_directory) ;

    } ;

}

#endif

