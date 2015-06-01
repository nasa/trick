/*
   PURPOSE: ( Handle the simulation command line args )

   REFERENCE: ( Trick Simulation Environment )

   ASSUMPTIONS AND LIMITATIONS: ( None )

   CLASS: ( N/A )

   LIBRARY DEPENDENCY: ( None )

   PROGRAMMERS: ( Keith Vetter LinCom 6/2003 ) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trick/Executive.hh"
#include "trick/command_line_protos.h"

/**
@details
-# If there are command line arguments
   -# If the argument is "trick_version" print out the verion of Trick used to build the
      simulation and exit.
   -# If the argument is "help" print out a help message about the possible command line
      arguments and exit.
*/
int Trick::Executive::process_sim_args() {

    char buf[4096] ;
    int argc ;
    char ** argv ;

    argc = command_line_args_get_argc() ;
    argv = command_line_args_get_argv() ;

    /* Save off a usage statement for messages */
    sprintf(buf, "%s", "\n\n"
            "USAGE:  S_main_<host_cpu>.exe  [option...]\n\n"
            "     RUN_<name>/<input_file>\n"
            "                         Every Trick simulation needs an input file.\n"
            "                         By convention, the input file is placed in\n"
            "                         a RUN_* directory.\n\n"
            "     RUN_<name>/<input_file>  -d\n"
            "                         Print input flags/parameters to the screen\n\n"
            "     RUN_<name>/<input_file>  [-O|-OO]  <output_file_path>\n"
            "                         Redirect all \"O\"utput files (log files) to the\n"
            "                         given directory.  If this option is not used, then\n"
            "                         all output files are placed by default in the\n"
            "                         RUN_<name> directory.\n\n"
            "     sie                 Generate the S_sie.resource file\n\n"
            "     trick_version       Print which version of Trick is being used\n"
            "                         to the screen.\n" ) ;

    /* If there are arguments to main... */
    if (argc > 1) {

        if (!strcmp(argv[1], "trick_version")) {
            /* Print the trick version to the screen */
            fprintf(stdout, "%s\n", current_version.c_str());
            exit(0);
        } else if (!strcmp(argv[1], "-help")  || !strcmp(argv[1], "--help") ||
                   !strcmp(argv[1], "-h")     || !strcmp(argv[1], "help") ) {
            /* Try and help the user */
            exit(0);
        }
    }

    return(0) ;
}

