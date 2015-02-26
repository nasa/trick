#include <cmath>

#include "sim_services/JITInputFile/include/jit_input_file_proto.hh"
#include "sim_services/JITInputFile/include/JITInputFile.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/EventManager/include/EventManager_c_intf.hh"

extern Trick::JITInputFile * the_jit ;

int jit_compile(std::string file_name) {
    if ( the_jit != NULL ) {
        return the_jit->compile(file_name) ;
    }
    return -1 ;
}

int jit_run(std::string library_name , std::string run_function ) {
    if ( the_jit != NULL ) {
        return the_jit->run(library_name, run_function) ;
    }
    return -1 ;
}

int jit_compile_and_run(std::string file_name, std::string run_function ) {
    if ( the_jit != NULL ) {
        return the_jit->compile_and_run(file_name, run_function) ;
    }
    return -1 ;
}

int jit_add_library(std::string lib_name ) {
    if ( the_jit != NULL ) {
        return the_jit->add_library(lib_name) ;
    }
    return -1 ;
}

int jit_add_read( unsigned int thread_id , double in_time , int (*func_ptr)(void) ) {
    Trick::JITEvent * event = new Trick::JITEvent(func_ptr, "jit_read_event" , 0) ;
    event->set_thread(thread_id) ;
    event->set_next_tics((long long)round(in_time * exec_get_time_tic_value())) ;
    return event_manager_add_event(event) ;
}

int jit_add_read( double in_time , int (*func_ptr)(void) ) {
    return jit_add_read( 0 , in_time , func_ptr) ;
}

int jit_add_event( int (*func_ptr)(void), std::string name , double cycle ) {
    Trick::JITEvent * my_event = new Trick::JITEvent( func_ptr, name , cycle ) ;
    return event_manager_add_event(my_event) ;
}

int jit_add_event_before( int (*func_ptr)(void), std::string name , std::string target_name , unsigned int target_inst ) {
    Trick::JITEvent * my_event = new Trick::JITEvent( func_ptr, name ) ;
    return event_manager_add_event_before(my_event, target_name , target_inst) ;
}

int jit_add_event_after( int (*func_ptr)(void), std::string name , std::string target_name , unsigned int target_inst ) {
    Trick::JITEvent * my_event = new Trick::JITEvent( func_ptr, name ) ;
    return event_manager_add_event_after(my_event, target_name , target_inst) ;
}
