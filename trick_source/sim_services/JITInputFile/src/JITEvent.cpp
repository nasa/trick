
#include "sim_services/JITInputFile/include/JITEvent.hh"
#include "sim_services/Executive/include/exec_proto.h"

Trick::JITEvent::JITEvent(int (*in_func_ptr)(void), std::string in_name , double in_cycle ) :
 Trick::Event(in_name, in_cycle),
 func_ptr(in_func_ptr) {
    // Set active to true by default for all JITEvents
    active = true ;
 }

/**
@details
-# call the function and return it's return value.
*/
int Trick::JITEvent::process( long long curr_time __attribute__ ((unused)) ) {
    return (*func_ptr)() ;
}

