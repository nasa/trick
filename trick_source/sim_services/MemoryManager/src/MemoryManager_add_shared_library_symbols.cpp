/*
   PURPOSE: ( Add a shared library )
   REFERENCE:
   ASSUMPTIONS AND LIMITATIONS:
   PROGRAMMERS: ( (Alex Lin) (NASA) (3/17/03) )
 */

#include <iostream>
#include <dlfcn.h>

#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

/* This routine only returns the size if ptr points to the beginning of the allocation area */
int Trick::MemoryManager::add_shared_library_symbols(const char * file_name) {

    void * new_handle ;

    new_handle = dlopen( file_name , RTLD_LAZY ) ;
    if ( new_handle != NULL ) {
        dlhandles.push_back(new_handle) ;
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: add_shared_library_symbols could not find library file: %s.\n", file_name) ;
    }

    return 0 ;
}

