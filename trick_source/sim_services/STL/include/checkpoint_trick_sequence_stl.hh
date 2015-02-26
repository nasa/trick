
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef __CHECKPOINT_TRICK_SEQUENCE_STL_HH__
#define __CHECKPOINT_TRICK_SEQUENCE_STL_HH__

#include <string>
#include <iostream>
#include <algorithm>
#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/STL/include/STLInterface.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

/*
   These template routines perform the checkpoint and restore work for sequence STL types.
   The sequence STL types include vectors, lists, deques, sets, and multisets.
   There are 3 checkpoint and restore versions depending on the contained type.
 */

template <class STL>
int checkpoint_trick_sequence_stl(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    typename STL::value_type * items = NULL ;
    typename STL::iterator it ;
    typename STL::iterator end ;

    cont_size = in_stl.size() ;

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%06d[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id, cont_size) ;
        items = (typename STL::value_type *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL with %s\n", var_declare) ;

        /* copy the contents of the stl */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            items[ii] = *it ;
        }
    }

    return 0 ;
}

template <class STL>
int checkpoint_trick_sequence_stl_string(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    const char ** items ;
    typename STL::iterator it ;
    typename STL::iterator end ;

    cont_size = in_stl.size() ;

    if ( cont_size > 0 ) {

        sprintf(var_declare, "char * %s_%06d[%d]" , object_name.c_str(), id , cont_size) ;
        items = (const char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL_STRING with %s\n", var_declare) ;

        /* copy the contents of the vector */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            items[ii] = it->c_str() ;
        }
    }

    return 0 ;
}

template <class STL>
int checkpoint_trick_sequence_stl_STLInterface(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    unsigned int * items = NULL ;
    typename STL::iterator it ;
    typename STL::iterator end ;
    typename STL::value_type vt ;

    cont_size = in_stl.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "unsigned int %s_%06d[%d]" , object_name.c_str(), id, cont_size) ;
        items = (unsigned int *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL with %s\n", var_declare) ;

            /* copy the contents of the stl */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            items[ii] = (*it).stl_id ;
        }
    }

    return 0 ;
}

template <class STL>
int restore_trick_sequence_stl(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * items_ref ;
    typename STL::value_type * items ;

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    in_stl.clear() ;

    sprintf(var_declare, "%s_%06d" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( items_ref != NULL ) {
        items = (typename STL::value_type *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.insert( in_stl.end(), items[ii] ) ;
        }
    }

    return 0 ;
}

template <class STL>
int restore_trick_sequence_stl_string(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * items_ref ;
    char ** items ;

    //message_publish(1, "in specialized vector template restore\n") ;
    in_stl.clear() ;

    sprintf(var_declare, "%s_%06d" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( items_ref != NULL ) {
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.insert( in_stl.end(), items[ii] ) ;
        }
    }

    return 0 ;
}

template <class STL>
int restore_trick_sequence_stl_STLInterface(STL & in_stl , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * items_ref ;
    unsigned int * items ;

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    in_stl.clear() ;

    sprintf(var_declare, "%s_%06d" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( items_ref != NULL ) {
        items = (unsigned int *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::value_type vt ;
            vt.stl_id = items[ii] ;
            vt.restart( object_name ) ;
            in_stl.insert( in_stl.end(), vt ) ;
        }
    }

    return 0 ;
}

#endif
