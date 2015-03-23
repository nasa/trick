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

#ifndef COMMAND_LINE_PROTOS_H
#define COMMAND_LINE_PROTOS_H

#ifdef __cplusplus
extern "C" {
#endif

int command_line_args_process_sim_args(int argc , char ** argv) ;

int command_line_args_get_argc() ;
char ** command_line_args_get_argv() ;
const char * command_line_args_get_output_dir() ;
const char * command_line_args_get_user_output_dir() ;
const char * command_line_args_get_input_file() ;

const char * command_line_args_get_default_dir() ;
const char * command_line_args_get_cmdline_name() ;

int output_dir_timestamped_on() ;
int output_dir_timestamped_off() ;

void set_output_dir(const char* output_directory);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <string>
std::string & command_line_args_get_input_file_ref() ;
#endif

#endif

