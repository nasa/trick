
#include <iostream>
#include <sstream>

#include "trick/Executive.hh"
#include "trick/Exec_exception.hh"

int Trick::Executive::stop() {

    /** @par Detailed Design */
    /** @li Set exec_command to ExitCmd.  Requirement [@ref r_exec_mode_4] */
    exec_command = ExitCmd ;
    //throw Trick::Exec_exception(-1 , __FILE__ , __LINE__ , "Executive::stop command called" ) ;

    return(0) ;

}

int Trick::Executive::stop(double in_time) {

    /** @par Detailed Design */
    /** @li Set exec_command to ExitCmd.  Requirement [@ref r_exec_mode_4] */
    set_terminate_time(in_time) ;

    return(0) ;

}
