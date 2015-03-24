
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_STACK_HH
#define CHECKPOINT_STACK_HH

#include <stack>
#include <algorithm>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/CheckPointRestart/include/checkpoint_sequence_stl.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

#ifndef TRICK_ICG
template <class ITEM_TYPE>
int delete_stl(std::stack<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::stack<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    ITEM_TYPE * items ;
    std::stack<ITEM_TYPE> temp_stack ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ; 

        temp_stack = in_stl ; 

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = temp_stack.top() ;
            temp_stack.pop() ;
        }
    }

    return 0 ;
}

/* Find the arrays the map data was stored in the checkpoint using ref_attributes 
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.  
 */
template <class ITEM_TYPE>
int restore_stl(std::stack<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    ITEM_TYPE * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_STACK %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    cont_size = in_stl.size() ;
    for ( ii = 0 ; ii < cont_size ; ii++ ) {
        in_stl.pop() ; 
    }

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ; 

    if ( items_ref != NULL ) {
        items = (ITEM_TYPE *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = cont_size - 1 ; ii < cont_size ; ii-- ) {
            in_stl.push( items[ii] ) ;
        } 
        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

// Specialized routines for strings.
int checkpoint_stl(std::stack<std::string> & in_stl , std::string object_name , std::string var_name ) ;
int restore_stl(std::stack<std::string> & in_stl , std::string object_name , std::string var_name ) ;

#endif

#endif
