#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"

int Trick::Executive::exec_terminate(const char *file_name, const char *error) {
    print_stack_trace_file();
    exec_command = ExitCmd ;
    throw Trick::ExecutiveException(-1 , file_name , 0 , error) ;
    return -1 ;
}
