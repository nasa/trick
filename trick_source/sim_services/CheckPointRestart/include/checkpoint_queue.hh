
/*
    PURPOSE: (Helpers to checkpoint STLs) 
*/

#ifndef __CHECKPOINT_QUEUE_HH__
#define __CHECKPOINT_QUEUE_HH__

#include <queue>
#include <algorithm>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/CheckPointRestart/include/checkpoint_sequence_stl.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

#ifndef TRICK_ICG
template <class ITEM_TYPE>
int delete_stl(std::queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::priority_queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::priority_queue<ITEM_TYPE, std::vector<ITEM_TYPE>, std::greater<ITEM_TYPE> > & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    ITEM_TYPE * items ;
    std::queue<ITEM_TYPE> temp_queue ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ; 

        temp_queue = in_stl ; 

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = temp_queue.front() ;
            temp_queue.pop() ;
        }
    }

    return 0 ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::priority_queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    ITEM_TYPE * items ;
    std::priority_queue<ITEM_TYPE> temp_queue ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ; 

        temp_queue = in_stl ; 

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = temp_queue.top() ;
            temp_queue.pop() ;
        }
    }

    return 0 ;
}

// template for priority queues using greater comparator
template <class ITEM_TYPE>
int checkpoint_stl(std::priority_queue<ITEM_TYPE, std::vector<ITEM_TYPE>, std::greater<ITEM_TYPE> > & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    ITEM_TYPE * items = NULL ;
    std::priority_queue<ITEM_TYPE, std::vector<ITEM_TYPE>, std::greater<ITEM_TYPE> > temp_queue ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ; 

        temp_queue = in_stl ; 

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = temp_queue.top() ;
            temp_queue.pop() ;
        }
    }

    return 0 ;
}

/* Find the arrays the map data was stored in the checkpoint using ref_attributes 
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.  
 */
template <class STL>
int restore_queue_stl(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    typename STL::value_type * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    cont_size = in_stl.size() ;
    for ( ii = 0 ; ii < cont_size ; ii++ ) {
        in_stl.pop() ; 
    }

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ; 

    if ( items_ref != NULL ) {
        items = (typename STL::value_type *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.push( items[ii] ) ;
        } 
        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

template <class STL>
int restore_queue_stl_string(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    char ** items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    cont_size = in_stl.size() ;
    for ( ii = 0 ; ii < cont_size ; ii++ ) {
        in_stl.pop() ; 
    }

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ; 

    if ( items_ref != NULL ) {
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.push( items[ii] ) ;
        } 
        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

template <class ITEM_TYPE>
int restore_stl(std::queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_queue_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::priority_queue<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_queue_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::priority_queue<ITEM_TYPE, std::vector<ITEM_TYPE>, std::greater<ITEM_TYPE> > & in_stl , std::string object_name , std::string var_name ) {
    return restore_queue_stl( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings.
int checkpoint_stl(std::queue<std::string> & in_stl , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::priority_queue<std::string> & in_stl , std::string object_name , std::string var_name ) ;
int restore_stl(std::queue<std::string> & in_stl , std::string object_name , std::string var_name ) ;
int restore_stl(std::priority_queue<std::string> & in_stl , std::string object_name , std::string var_name ) ;

#endif

#endif
