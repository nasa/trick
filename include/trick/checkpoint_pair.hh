
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
    std::string temp_str;

    FIRST * first = nullptr ;
    SECOND * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    std::string type_string ;
    try {
        type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status )) ;
    } catch (const std::bad_typeid& e) {
        message_publish(1, "Error, having difficulty checkpointing %s.%s\n", object_name.c_str(), var_name.c_str()) ;
        return 0 ;
    }
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_first[1]" ;
    temp_str = var_declare.str();
    first = (FIRST *)TMM_declare_var_s(temp_str.c_str()) ;
    if ( first ) {
        temp_str = std::string(object_name + "_" + var_name + "_first");
        TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
        first[0] = in_pair.first ;

        var_declare.str("") ;
        var_declare.clear() ;
        try {
            type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status )) ;
        } catch (const std::bad_typeid& e) {
            message_publish(1, "Error, having difficulty checkpointing %s.%s\n", object_name.c_str(), var_name.c_str()) ;
            return 0 ;
        }
        var_declare << type_string << " "
         << object_name << "_" << var_name << "_second[1]" ;
        temp_str = var_declare.str();
        second = (SECOND *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( second ) {
            temp_str = std::string(object_name + "_" + var_name + "_second");
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
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

    std::string temp_str;

    FIRST * first = nullptr ;
    std::string * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    std::string type_string;
    try {
    type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*first).name(), 0, 0, &status )) ;
    } catch (const std::bad_typeid& e) {
        message_publish(1, "Error, having difficulty checkpointing %s.%s\n", object_name.c_str(), var_name.c_str()) ;
        return 0 ;
    }
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_first[1]" ;
    temp_str = var_declare.str();
    first = (FIRST *)TMM_declare_var_s(temp_str.c_str()) ;
    if ( first ) {
        temp_str = std::string(object_name + "_" + var_name + "_first") ;
        TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
        first[0] = in_pair.first ;

        var_declare.str("") ;
        var_declare.clear() ;
        var_declare << "std::string "
         << object_name << "_" << var_name << "_second[1]" ;
        temp_str = var_declare.str();
        second = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( second ) {
            temp_str = std::string(object_name + "_" + var_name + "_second");
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
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

    std::string  temp_str ;

    std::string * first = nullptr ;
    SECOND * second = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    var_declare << "std::string "
     << object_name << "_" << var_name << "_first[1]" ;
    temp_str = var_declare.str() ;
    first = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
    temp_str = std::string(object_name + "_" + var_name + "_first");
    TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
    checkpoint_stl( in_pair.first , object_name + "_" + var_name , "first"  ) ;

    var_declare.str("") ;
    var_declare.clear() ;
    std::string type_string;
    try {
    type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*second).name(), 0, 0, &status )) ;
    } catch (const std::bad_typeid& e) {
        message_publish(1, "Error, having difficulty checkpointing %s.%s\n", object_name.c_str(), var_name.c_str()) ;
        return 0 ;
    }
    var_declare << type_string << " "
     << object_name << "_" << var_name << "_second[1]" ;
    temp_str = var_declare.str();
    second = (SECOND *)TMM_declare_var_s(temp_str.c_str()) ;
    temp_str = std::string(object_name + "_" + var_name + "_second");
    TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
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

    std::string temp_str;

    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    var_declare << "std::string "
     << object_name << "_" << var_name << "_first[1]" ;
    temp_str = var_declare.str();
    first = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
    temp_str = std::string(object_name + "_" + var_name + "_first");
    TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
    checkpoint_stl( in_pair.first , object_name + "_" + var_name , "first"  ) ;

    var_declare.str("") ;
    var_declare.clear() ;
    var_declare << "std::string "
     << object_name << "_" << var_name << "_second[1]" ;
    temp_str = var_declare.str();
    second = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
    temp_str = std::string(object_name + "_" + var_name + "_second");
    TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
    checkpoint_stl( in_pair.second , object_name + "_" + var_name , "second"  ) ;

    return 0 ;
}

/* =================================================================================================*/

template <class FIRST, class SECOND>
int delete_stl(std::pair<FIRST, SECOND> & in_stl __attribute__ ((unused)) , std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    REF2 * items_ref ;
    std::string temp_str = object_name + "_" + var_name + "_first";
    items_ref = ref_attributes(temp_str.c_str()) ;
    if ( items_ref != NULL ) {
        temp_str = object_name + "_" + var_name + "_first";
        TMM_delete_var_n(temp_str.c_str() ) ;
        temp_str = object_name + "_" + var_name + "_second";
        TMM_delete_var_n(temp_str.c_str() ) ;
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

    std::string temp_str; 
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + "_" + var_name + "_first";
    first_ref = ref_attributes((char *)temp_str.c_str()) ;
    temp_str = object_name + "_" + var_name + "_second" ;
    second_ref = ref_attributes((char *)temp_str.c_str()) ;

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

    std::string temp_str;

    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + "_" + var_name + "_first";
    first_ref = ref_attributes((char *)temp_str.c_str()) ;
    temp_str = object_name + "_" + var_name + "_second";
    second_ref = ref_attributes((char *)temp_str.c_str()) ;

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

    std::string temp_str; 
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + "_" + var_name + "_first";
    first_ref = ref_attributes((char *)temp_str.c_str()) ;
    temp_str = object_name + "_" + var_name + "_second";
    second_ref = ref_attributes((char *)temp_str.c_str()) ;

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

    std::string temp_str;

    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + "_" + var_name + "_first";
    first_ref = ref_attributes((char *)temp_str.c_str()) ;
    temp_str = object_name + "_" + var_name + "_second" ;
    second_ref = ref_attributes((char *)temp_str.c_str()) ;

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
