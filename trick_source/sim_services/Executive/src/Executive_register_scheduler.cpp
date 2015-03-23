
#include <iostream>
#include <sstream>

#include "sim_services/Executive/include/Executive.hh"

int Trick::Executive::register_scheduler( Trick::Scheduler * scheduler ) {
    other_schedulers.push_back(scheduler) ;
    return(0) ;

}
