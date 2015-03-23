/*
    PURPOSE: (Illustrate how to checkpoint STLs)
*/

#ifndef CHECKPOINT_TRICK_MAP_HH
#define CHECKPOINT_TRICK_MAP_HH

#include <string>
#include <algorithm>
#include <typeinfo>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/STL/include/STLInterface.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

/* For all of the checkpoint_map_stl variations.  Declare 2 memory manager arrays to hold
   the contents of the map. No simulation variable actually points to these arrays.  Only the
   user knows they are there and how they are stored.  The restart function knows how to
   "retrieve" and restore the data.  The name of the declaration is important.  Each instance of
   the STL requires a unique name to be saved correctly.  The incoming "object_name" and "var_name"
   gives that unique name.
 */
template <class STL>
int checkpoint_trick_map_nkey_ndata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys ;
    typename STL::mapped_type * items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "%s %s_%06d_keys[%d]" ,
         abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id, cont_size) ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "%s %s_%06d_data[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id, cont_size) ;
        items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = iter->first ;
            items[ii] = iter->second ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_nkey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys ;
    char ** items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "%s %s_%06d_keys[%d]" ,
         abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id , cont_size) ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "char * %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = iter->first ;
            items[ii] = (char *)((iter->second).c_str()) ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_nkey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys ;
    unsigned int * items = NULL ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "%s %s_%06d_keys[%d]" ,
         abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), id , cont_size) ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "unsigned int %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (unsigned int*)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = iter->first ;
            items[ii] = (iter->second).stl_id ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_skey_ndata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    char ** keys ;
    typename STL::mapped_type * items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "char * %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (char **)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "%s %s_%06d_data[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id , cont_size) ;
        items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (char *)((iter->first).c_str()) ;
            items[ii] = iter->second ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_skey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;

    char ** keys ;
    char ** items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "char * %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (char **)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "char * %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */

        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (char *)((iter->first).c_str()) ;
            items[ii] = (char *)((iter->second).c_str()) ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_skey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;

    char ** keys ;
    unsigned int * items = NULL ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "char * %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (char **)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "unsigned int %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (unsigned int*)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (char *)((iter->first).c_str()) ;
            items[ii] = (iter->second).stl_id ;
        }
    }
    return 0 ;
}


template <class STL>
int checkpoint_trick_map_stlkey_ndata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    unsigned int * keys = NULL ;
    typename STL::mapped_type * items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "unsigned int %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "%s %s_%06d_data[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), id , cont_size) ;
        items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (iter->first).stl_id ;
            items[ii] = iter->second ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_stlkey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;

    unsigned int * keys = NULL ;
    char ** items ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "unsigned int %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "char * %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (iter->first).stl_id ;
            items[ii] = (char *)((iter->second).c_str()) ;
        }
    }
    return 0 ;
}

template <class STL>
int checkpoint_trick_map_stlkey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;

    unsigned int * keys = NULL ;
    unsigned int * items = NULL ;

    cont_size = in_map.size() ;

    if ( cont_size > 0 ) {
        sprintf(var_declare, "unsigned int %s_%06d_keys[%d]" , object_name.c_str(), id , cont_size) ;
        keys = (unsigned int*)TMM_declare_var_s(var_declare) ;

        sprintf(var_declare, "unsigned int %s_%06d_data[%d]" , object_name.c_str(), id , cont_size) ;
        items = (unsigned int*)TMM_declare_var_s(var_declare) ;

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (iter->first).stl_id ;
            items[ii] = (iter->second).stl_id ;
        }
    }
    return 0 ;
}

/* For all restore_map_stl varieties.
   Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number
   of items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */
template <class STL>
int restore_trick_map_nkey_ndata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    typename STL::mapped_type * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<typename STL::key_type , typename STL::mapped_type>(keys[ii], items[ii])) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_nkey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    char ** items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<typename STL::key_type, std::string>(keys[ii] , std::string(items[ii]))) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_nkey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    unsigned int * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::mapped_type mt ;
            mt.stl_id = items[ii] ;
            mt.restart( object_name ) ;
            in_map.insert( std::pair<typename STL::key_type, typename STL::mapped_type>(keys[ii] , mt)) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_skey_ndata( STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    typename STL::mapped_type * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<std::string , typename STL::mapped_type>(std::string(keys[ii]), items[ii])) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_skey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    char ** items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<std::string, std::string>(std::string(keys[ii]) , std::string(items[ii]))) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_skey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    unsigned int * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::mapped_type mt ;
            mt.stl_id = items[ii] ;
            mt.restart( object_name ) ;
            in_map.insert( std::pair<std::string, typename STL::mapped_type>(std::string(keys[ii]) , mt)) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_stlkey_ndata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    typename STL::mapped_type * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::key_type kt ;
            kt.stl_id = keys[ii] ;
            kt.restart( object_name ) ;
            in_map.insert( std::pair<typename STL::key_type, typename STL::mapped_type>(kt , items[ii])) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_stlkey_sdata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    char ** items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::key_type kt ;
            kt.stl_id = keys[ii] ;
            kt.restart( object_name ) ;
            in_map.insert( std::pair<typename STL::key_type, typename STL::mapped_type>(kt , std::string(items[ii]))) ;
        }
    }
    return 0 ;
}

template <class STL>
int restore_trick_map_stlkey_stldata(STL & in_map , std::string object_name , unsigned int id ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    REF2 * keys_ref , * items_ref ;
    unsigned int * keys ;
    unsigned int * items ;

    in_map.clear() ;

    sprintf(var_declare, "%s_%06d_keys" , object_name.c_str(), id ) ;
    keys_ref = ref_attributes(var_declare) ;
    sprintf(var_declare, "%s_%06d_data" , object_name.c_str(), id ) ;
    items_ref = ref_attributes(var_declare) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (unsigned int *)keys_ref->address ;
        items = (unsigned int *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::key_type kt ;
            kt.stl_id = keys[ii] ;
            kt.restart( object_name ) ;
            typename STL::mapped_type mt ;
            mt.stl_id = items[ii] ;
            mt.restart( object_name ) ;
            in_map.insert( std::pair<typename STL::key_type, typename STL::mapped_type>(kt , mt)) ;
        }
    }
    return 0 ;
}

#endif
