
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_PAIR_HH
#define CHECKPOINT_PAIR_HH

#include <utility>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include <functional>
#include <string>
#include <type_traits>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "checkpoint_is_stl_container.hh"
#include "checkpoint_stl_protos.hh"
#include "checkpoint_fwd_declare.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"

// intrinsic first , intrinsic second
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {
    std::ostringstream var_declare ;
    int status ;

    FIRST * first = nullptr ;
    SECOND * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status )) ;
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_first[1]" ;
    first = (FIRST *)TMM_declare_var_s(var_declare.str().c_str()) ;
    if ( first ) {
        TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_first").c_str()) ;
        first[0] = in_pair.first ;

        var_declare.str("") ;
        var_declare.clear() ;
        type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status )) ;
        var_declare << type_string << " "
         << object_name << "_" << var_name << "_second[1]" ;
        second = (SECOND *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( second ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_second").c_str()) ;
            second[0] = in_pair.second ;
        }
    }

    return 0 ;
}

// intrinsic first , STL second
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    std::ostringstream var_declare ;
    int status ;

    FIRST * first = nullptr ;
    std::string * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status )) ;
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_first[1]" ;
    first = (FIRST *)TMM_declare_var_s(var_declare.str().c_str()) ;
    if ( first ) {
        TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_first").c_str()) ;
        first[0] = in_pair.first ;

        var_declare.str("") ;
        var_declare.clear() ;
        var_declare << "std::string "
         << object_name << "_" << var_name << "_second[1]" ;
        second = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( second ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_second").c_str()) ;
            checkpoint_stl( in_pair.second , object_name + "_" + var_name , "second"  ) ;
        }
    }

    return 0 ;
}

// STL first , intrinsic second
template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    std::ostringstream var_declare ;
    int status ;

    std::string * first = nullptr ;
    SECOND * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    var_declare << "std::string "
     << object_name << "_" << var_name << "_first[1]" ;
    first = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_first").c_str()) ;
    checkpoint_stl( in_pair.first , object_name + "_" + var_name , "first"  ) ;

    var_declare.str("") ;
    var_declare.clear() ;
    std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status )) ;
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_second[1]" ;
    second = (SECOND *)TMM_declare_var_s(var_declare.str().c_str()) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_second").c_str()) ;
    second[0] = in_pair.second ;

    return 0 ;
}

// STL first , STL second
template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    std::ostringstream var_declare ;

    std::string * first = nullptr ;
    std::string * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    var_declare << "std::string "
     << object_name << "_" << var_name << "_first[1]" ;
    first = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_first").c_str()) ;
    checkpoint_stl( in_pair.first , object_name + "_" + var_name , "first"  ) ;

    var_declare.str("") ;
    var_declare.clear() ;
    var_declare << "std::string "
     << object_name << "_" << var_name << "_second[1]" ;
    second = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_second").c_str()) ;
    checkpoint_stl( in_pair.second , object_name + "_" + var_name , "second"  ) ;

    return 0 ;
}

/* =================================================================================================*/

template <class FIRST, class SECOND>
int delete_stl(std::pair<FIRST, SECOND> & in_stl __attribute__ ((unused)) , std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    REF2 * items_ref ;
    items_ref = ref_attributes((char *)(object_name + "_" + var_name + "_first").c_str()) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n((object_name + "_" + var_name + "_first").c_str() ) ;
        TMM_delete_var_n((object_name + "_" + var_name + "_second").c_str() ) ;
        free(items_ref) ;
    }
    return 0 ;
}

/* =================================================================================================*/

// intrinsic first, intrinsic second
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    FIRST * first ;
    SECOND * second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + "_" + var_name + "_first").c_str()) ;
    second_ref = ref_attributes((char *)(object_name + "_" + var_name + "_second").c_str()) ;

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (FIRST *)first_ref->address ;
        second = (SECOND *)second_ref->address ;

        in_pair.first = first[0] ;
        in_pair.second = second[0] ;

        delete_stl( in_pair , object_name , var_name ) ;
    }

    return 0 ;
}

// intrinsic first, STL second
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    FIRST * first ;
    std::string * second ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + "_" + var_name + "_first").c_str()) ;
    second_ref = ref_attributes((char *)(object_name + "_" + var_name + "_second").c_str()) ;

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (FIRST *)first_ref->address ;
        second = (std::string *)second_ref->address ;

        in_pair.first = first[0] ;
        restore_stl( in_pair.second , object_name + "_" + var_name , "second" ) ;

        delete_stl( in_pair , object_name , var_name ) ;
    }

    return 0 ;
}

// STL first, intrinsic second
template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    std::string * first ;
    SECOND * second ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + "_" + var_name + "_first").c_str()) ;
    second_ref = ref_attributes((char *)(object_name + "_" + var_name + "_second").c_str()) ;

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (std::string *)first_ref->address ;
        second = (SECOND *)second_ref->address ;

        restore_stl( in_pair.first , object_name + "_" + var_name , "first" ) ;
        in_pair.second = second[0] ;

        delete_stl( in_pair , object_name , var_name ) ;
    }

    return 0 ;
}

// STL first, STL second
template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) {
    REF2 * first_ref ;
    REF2 * second_ref ;

    std::string * first ;
    std::string * second ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + "_" + var_name + "_first").c_str()) ;
    second_ref = ref_attributes((char *)(object_name + "_" + var_name + "_second").c_str()) ;

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (std::string *)first_ref->address ;
        second = (std::string *)second_ref->address ;

        restore_stl( in_pair.first , object_name + "_" + var_name , "first" ) ;
        restore_stl( in_pair.second , object_name + "_" + var_name , "second" ) ;

        delete_stl( in_pair , object_name , var_name ) ;
    }

    return 0 ;
}

#endif
