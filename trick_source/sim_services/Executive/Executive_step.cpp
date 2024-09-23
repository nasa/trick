
#include <iostream>
#include <sstream>

#include "trick/Executive.hh"
#include "trick/Message_proto.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/DebugPause.hh"

int Trick::Executive::step() {

    /** @par Detailed Design */
    /** @li Set exec_command to RunCmd.  Requirement [@ref r_exec_mode_5] */
    exec_command = StepCmd ;
    
    return(0) ;

}
