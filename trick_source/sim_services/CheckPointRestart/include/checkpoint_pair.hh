
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_PAIR_HH
#define CHECKPOINT_PAIR_HH

#include <utility>
#include <algorithm>
#include <typeinfo>
#include <functional>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

#ifndef TRICK_ICG
template <class FIRST_TYPE, class SECOND_TYPE>
int delete_stl(std::pair<FIRST_TYPE, SECOND_TYPE> & in_stl __attribute__ ((unused)) , std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    REF2 * items_ref ;
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_first")).c_str()) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n((object_name + std::string("_") + var_name + std::string("_first")).c_str() ) ;
        TMM_delete_var_n((object_name + std::string("_") + var_name + std::string("_second")).c_str() ) ;
        free(items_ref) ;
    }
    return 0 ;
}

template <class FIRST_TYPE, class SECOND_TYPE>
int checkpoint_stl(std::pair<FIRST_TYPE, SECOND_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    char var_declare[128] ;
    int status ;

    FIRST_TYPE * first ;
    SECOND_TYPE * second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    sprintf(var_declare, "%s %s_%s_first[1]" ,
     abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str()) ;
    first = (FIRST_TYPE *)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "%s %s_%s_second[1]" ,
     abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str()) ;
    second = (SECOND_TYPE *)TMM_declare_var_s(var_declare) ;

    first[0] = in_stl.first ;
    second[0] = in_stl.second ;

    return 0 ;
}

template <class FIRST_TYPE>
int checkpoint_stl(std::pair<FIRST_TYPE, std::string> & in_stl , std::string object_name , std::string var_name ) {

    char var_declare[128] ;
    int status ;

    FIRST_TYPE * first ;
    char ** second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    sprintf(var_declare, "%s %s_%s_first[1]" ,
     abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str()) ;
    first = (FIRST_TYPE *)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "char * %s_%s_second[1]" , object_name.c_str(), var_name.c_str()) ;
    second = (char **)TMM_declare_var_s(var_declare) ;

    first[0] = in_stl.first ;
    second[0] = (char *)(in_stl.second.c_str()) ;

    return 0 ;
}

template <class SECOND_TYPE>
int checkpoint_stl(std::pair<std::string, SECOND_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    char var_declare[128] ;
    int status ;

    char ** first ;
    SECOND_TYPE * second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    sprintf(var_declare, "char * %s_%s_first[1]", object_name.c_str(), var_name.c_str()) ;
    first = (char **)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "%s %s_%s_second[1]" ,
     abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str()) ;
    second = (SECOND_TYPE *)TMM_declare_var_s(var_declare) ;

    first[0] = (char *)(in_stl.first.c_str()) ;
    second[0] = in_stl.second ;

    return 0 ;
}

template <class FIRST_TYPE, class SECOND_TYPE>
int restore_stl(std::pair<FIRST_TYPE, SECOND_TYPE> & in_stl , std::string object_name , std::string var_name ) {


    REF2 * first_ref ;
    REF2 * second_ref ;

    FIRST_TYPE * first ;
    SECOND_TYPE * second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_first")).c_str()) ; 
    second_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_second")).c_str()) ; 

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (FIRST_TYPE *)first_ref->address ;
        second = (SECOND_TYPE *)second_ref->address ;

        in_stl.first = first[0] ;
        in_stl.second = second[0] ;

        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

template <class FIRST_TYPE>
int restore_stl(std::pair<FIRST_TYPE, std::string > & in_stl , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    FIRST_TYPE * first ;
    char ** second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_first")).c_str()) ; 
    second_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_second")).c_str()) ; 

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (FIRST_TYPE *)first_ref->address ;
        second = (char **)second_ref->address ;

        in_stl.first = first[0] ;
        in_stl.second = std::string(second[0]) ;

        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

template <class SECOND_TYPE>
int restore_stl(std::pair<std::string, SECOND_TYPE> & in_stl , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    char ** first ;
    SECOND_TYPE * second ;
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    first_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_first")).c_str()) ; 
    second_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_second")).c_str()) ; 

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (char **)first_ref->address ;
        second = (SECOND_TYPE *)second_ref->address ;

        in_stl.first = std::string(first[0]) ;
        in_stl.second = second[0] ;

        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

// Specialized routines for strings.
int checkpoint_stl(std::pair<std::string, std::string> & in_stl , std::string object_name , std::string var_name ) ;
int restore_stl(std::pair<std::string, std::string> & in_stl , std::string object_name , std::string var_name ) ;

#endif

#endif
