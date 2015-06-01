
#include <iostream>
#include <sstream>

#include "trick/Executive.hh"
#include "trick/Exec_exception.hh"
#include "trick/exec_proto.h"

int Trick::Executive::exec_terminate(const char *file_name, const char *error) {
    exec_command = ExitCmd ;
    throw Trick::Exec_exception(-1 , file_name , 0 , error) ;
    return -1 ;
}
