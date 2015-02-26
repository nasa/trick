/*
    PURPOSE: (Illustrate how to checkpoint STLs)
*/

#ifndef __CHECKPOINT_TRICK_PAIR_HH__
#define __CHECKPOINT_TRICK_PAIR_HH__

#include <string>
#include <algorithm>
#include <typeinfo>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/STL/include/STLInterface.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

template <class STL>
int checkpoint_trick_pair_nkey_ndata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;
    int status ;

    typename STL::first_type * keys ;
    typename STL::second_type * items ;

    sprintf(var_declare, "%s %s_%06d_keys[1]" ,
     abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id) ;
    keys = (typename STL::first_type *)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "%s %s_%06d_data[1]" ,
     abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id) ;
    items = (typename STL::second_type *)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = in_pair.first ;
    items[0] = in_pair.second ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_nkey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;
    int status ;

    typename STL::first_type * keys ;
    char ** items ;

    sprintf(var_declare, "%s %s_%06d_keys[1]" ,
     abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id ) ;
    keys = (typename STL::first_type *)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "char * %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (char **)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = in_pair.first ;
    items[0] = (char *)((in_pair.second).c_str()) ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_nkey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;
    int status ;

    typename STL::first_type * keys ;
    unsigned int * items = NULL ;

    sprintf(var_declare, "%s %s_%06d_keys[1]" ,
     abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id ) ;
    keys = (typename STL::first_type *)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "unsigned int %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (unsigned int*)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = in_pair.first ;
    items[0] = (in_pair.second).stl_id ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_skey_ndata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;
    int status ;

    char ** keys ;
    typename STL::second_type * items ;

    sprintf(var_declare, "char * %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (char **)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "%s %s_%06d_data[1]" ,
     abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id ) ;
    items = (typename STL::second_type *)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (char *)((in_pair.first).c_str()) ;
    items[0] = in_pair.second ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_skey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    char ** keys ;
    char ** items ;

    sprintf(var_declare, "char * %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (char **)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "char * %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (char **)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (char *)((in_pair.first).c_str()) ;
    items[0] = (char *)((in_pair.second).c_str()) ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_skey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    char ** keys ;
    unsigned int * items = NULL ;

    sprintf(var_declare, "char * %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (char **)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "unsigned int %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (unsigned int*)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (char *)((in_pair.first).c_str()) ;
    items[0] = (in_pair.second).stl_id ;

    return 0 ;
}


template <class STL>
int checkpoint_trick_pair_stlkey_ndata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;
    int status ;

    unsigned int * keys = NULL ;
    typename STL::second_type * items ;

    sprintf(var_declare, "unsigned int %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "%s %s_%06d_data[1]" ,
     abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id ) ;
    items = (typename STL::second_type *)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (in_pair.first).stl_id ;
    items[0] = in_pair.second ;

    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_stlkey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    unsigned int * keys = NULL ;
    char ** items ;

    sprintf(var_declare, "unsigned int %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "char * %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (char **)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (in_pair.first).stl_id ;
    items[0] = (char *)((in_pair.second).c_str()) ;
    return 0 ;
}

template <class STL>
int checkpoint_trick_pair_stlkey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    unsigned int * keys = NULL ;
    unsigned int * items = NULL ;

    sprintf(var_declare, "unsigned int %s_%06d_keys[1]" , object_name.c_str(), id ) ;
    keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

    sprintf(var_declare, "unsigned int %s_%06d_data[1]" , object_name.c_str(), id ) ;
    items = (unsigned int*)TMM_declare_var_s(var_declare) ;

    /* copy the contents of the pair the 2 arrays */
    keys[0] = (in_pair.first).stl_id ;
    items[0] = (in_pair.second).stl_id ;
    return 0 ;
}

/* RESTORE */

template <class STL>
int restore_trick_pair_nkey_ndata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::first_type * keys ;
    typename STL::second_type * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::first_type *)keys_ref->address ;
        items = (typename STL::second_type *)items_ref->address ;

        in_pair.first = keys[0] ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_nkey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::first_type * keys ;
    char ** items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::first_type *)keys_ref->address ;
        items = (char **)items_ref->address ;

        in_pair.first = keys[0] ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_nkey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::first_type * keys ;
    unsigned int * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::first_type *)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;

        typename STL::second_type mt ;
        mt.stl_id = items[0] ;
        mt.restart( object_name ) ;

        in_pair.first = keys[0] ;
        in_pair.second = mt ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_skey_ndata( STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    typename STL::second_type * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (typename STL::second_type *)items_ref->address ;

        in_pair.first = keys[0] ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_skey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    char ** items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (char **)items_ref->address ;

        in_pair.first = keys[0] ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_skey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    unsigned int * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;

        typename STL::second_type mt ;
        mt.stl_id = items[0] ;
        mt.restart( object_name ) ;

        in_pair.first = keys[0] ;
        in_pair.second = mt ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_stlkey_ndata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    typename STL::second_type * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (typename STL::second_type *)items_ref->address ;

        typename STL::first_type kt ;
        kt.stl_id = keys[0] ;
        kt.restart( object_name ) ;

        in_pair.first = kt ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_stlkey_sdata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    char ** items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (char **)items_ref->address ;

        typename STL::first_type kt ;
        kt.stl_id = keys[0] ;
        kt.restart( object_name ) ;

        in_pair.first = kt ;
        in_pair.second = items[0] ;
    }
    return 0 ;
}

template <class STL>
int restore_trick_pair_stlkey_stldata(STL & in_pair , std::string object_name , unsigned int id ) {

    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    unsigned int * items ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;

        typename STL::first_type kt ;
        kt.stl_id = keys[0] ;
        kt.restart( object_name ) ;
        typename STL::second_type mt ;
        mt.stl_id = items[0] ;
        mt.restart( object_name ) ;

        in_pair.first = kt ;
        in_pair.second = mt ;
    }
    return 0 ;
}

#endif
