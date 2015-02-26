/* 
 * $Id:$
 */

#include <iostream>
#include <sstream>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Message/include/Message_proto.hh"

int Trick::Executive::run() {

    /** @par Detailed Design */
    /** @li Set exec_command to RunCmd.  Requirement [@ref r_exec_mode_5] */
    exec_command = RunCmd ;
    return(0) ;

}
