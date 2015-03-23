
#include <iostream>
#include <sstream>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/Exec_exception.hh"
#include "sim_services/Executive/include/exec_proto.h"

int Trick::Executive::exec_terminate(const char *file_name, const char *error) {
    exec_command = ExitCmd ;
    throw Trick::Exec_exception(-1 , file_name , 0 , error) ;
    return -1 ;
}
