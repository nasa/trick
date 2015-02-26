/*
    PURPOSE: (Illustrate how to checkpoint STLs) 
    LIBRARY_DEPENDENCIES: (
      (checkpoint_map.o)
    )
*/

#ifndef __CHECKPOINT_MAP_HH__
#define __CHECKPOINT_MAP_HH__

#include <map>
#include <string>
#include <algorithm>
#include <typeinfo>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

#ifndef TRICK_ICG

/* For all of the checkpoint_map_stl variations.  Declare 2 memory manager arrays to hold
   the contents of the map. No simulation variable actually points to these arrays.  Only the
   user knows they are there and how they are stored.  The restart function knows how to
   "retrieve" and restore the data.  The name of the declaration is important.  Each instance of
   the STL requires a unique name to be saved correctly.  The incoming "object_name" and "var_name"
   gives that unique name.
 */
template <class STL>
int checkpoint_map_stl(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys ;
    typename STL::mapped_type * items ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        sprintf(var_declare, "%s %s_%s_keys[%d]" ,
         abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "HERE with %s\n", var_declare) ; 

        sprintf(var_declare, "%s %s_%s_data[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "HERE with %s\n", var_declare) ; 

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = iter->first ;
            items[ii] = iter->second ;
        }
    }
    return 0 ;
}

template <class MAP_KEY, class MAP_ITEM>
int checkpoint_stl(std::map<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl( in_map , object_name , var_name ) ;
}

template <class MAP_KEY, class MAP_ITEM>
int checkpoint_stl(std::multimap<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl( in_map , object_name , var_name ) ;
}

template <class STL>
int checkpoint_map_stl_key_string(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    char ** keys ;
    typename STL::mapped_type * items ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        sprintf(var_declare, "char * %s_%s_keys[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        keys = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        sprintf(var_declare, "%s %s_%s_data[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str() , cont_size) ;
        items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (char *)((iter->first).c_str()) ;
            items[ii] = iter->second ;
        }
    }
    return 0 ;
}

template <class MAP_ITEM>
int checkpoint_stl(std::map<std::string , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_key_string( in_map , object_name , var_name ) ;
}

template <class MAP_ITEM>
int checkpoint_stl(std::multimap<std::string , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_key_string( in_map , object_name , var_name ) ;
}

template <class STL>
int checkpoint_map_stl_data_string(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys ;
    char ** items ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        sprintf(var_declare, "%s %s_%s_keys[%d]" ,
         abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str() , cont_size) ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        sprintf(var_declare, "char * %s_%s_data[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        /* copy the contents of the map the 2 arrays */
        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = iter->first ;
            items[ii] = (char *)((iter->second).c_str()) ;
        }
    }
    return 0 ;
}


template <class MAP_KEYS>
int checkpoint_stl(std::map<MAP_KEYS, std::string> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_data_string( in_map , object_name , var_name ) ;
}

template <class MAP_KEYS>
int checkpoint_stl(std::multimap<MAP_KEYS, std::string> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_data_string( in_map , object_name , var_name ) ;
}

template <class STL>
int checkpoint_map_stl_strings(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    typename STL::iterator iter ;

    char ** keys ;
    char ** items ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        sprintf(var_declare, "char * %s_%s_keys[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        keys = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        sprintf(var_declare, "char * %s_%s_data[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "STRING KEY HERE with %s\n", var_declare) ; 

        /* copy the contents of the map the 2 arrays */

        for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
            keys[ii] = (char *)((iter->first).c_str()) ;
            items[ii] = (char *)((iter->second).c_str()) ;
        }
    }
    return 0 ;
}

int checkpoint_stl(std::map<std::string , std::string> & in_map , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::multimap<std::string , std::string> & in_map , std::string object_name , std::string var_name ) ;

template <class STL>
int delete_map_stl(STL & in_map __attribute__ ((unused)), std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    REF2 * items_ref ;
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_keys")).c_str()) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n((object_name + std::string("_") + var_name + std::string("_keys")).c_str() ) ;
        TMM_delete_var_n((object_name + std::string("_") + var_name + std::string("_data")).c_str() ) ;
        free(items_ref) ;
    }
    return 0 ;
}

template <class MAP_KEY, class MAP_ITEM>
int delete_stl(std::map<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return delete_map_stl(in_map , object_name , var_name) ;
}

template <class MAP_KEY, class MAP_ITEM>
int delete_stl(std::multimap<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return delete_map_stl(in_map , object_name , var_name) ;
}

/* For all restore_map_stl varieties.
   Find the arrays the map data was stored in the checkpoint using ref_attributes 
   From the address of the resulting ref_attributes, we can figure out the number
   of items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.  
 */
template <class STL>
int restore_map_stl(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    typename STL::mapped_type * items ;

    //message_publish(1, "in regular map template restore\n") ;

    in_map.clear() ; 
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_keys")).c_str()) ;
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_data")).c_str()) ; 

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<typename STL::key_type , typename STL::mapped_type>(keys[ii], items[ii])) ;
        }

        delete_stl( in_map , object_name , var_name ) ;
    } 
    return 0 ;
}

template <class MAP_KEY, class MAP_ITEM>
int restore_stl(std::map<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl(in_map , object_name , var_name) ;
    return 0 ;
}

template <class MAP_KEY, class MAP_ITEM>
int restore_stl(std::multimap<MAP_KEY , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_stl_key_string( STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    typename STL::mapped_type * items ;

    in_map.clear() ; 
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_keys")).c_str()) ; 
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_data")).c_str()) ; 

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<std::string , typename STL::mapped_type>(std::string(keys[ii]), items[ii])) ;
        }

        delete_stl( in_map , object_name , var_name ) ;
    } 
    return 0 ;
}

template <class MAP_ITEM>
int restore_stl(std::map<std::string , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_key_string(in_map , object_name , var_name) ;
}

template <class MAP_ITEM>
int restore_stl(std::multimap<std::string , MAP_ITEM> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_key_string(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_stl_data_string(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    char ** items ;

    //message_publish(1, "in specialized map template restore\n") ;

    in_map.clear() ; 
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_keys")).c_str()) ; 
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_data")).c_str()) ; 

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<typename STL::key_type, std::string>(keys[ii] , std::string(items[ii]))) ;
        }

        delete_stl( in_map , object_name , var_name ) ;
    } 
    return 0 ;
}

template <class MAP_KEY>
int restore_stl(std::map<MAP_KEY , std::string> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_data_string(in_map , object_name , var_name) ;
}

template <class MAP_KEY>
int restore_stl(std::multimap<MAP_KEY , std::string> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_data_string(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_stl_strings(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    char ** keys ;
    char ** items ;

    //message_publish(1, "in specialized map template restore\n") ;

    /* Find the arrays the map data was stored in the checkpoint using ref_attributes 
       From the address of the resulting ref_attributes, we can figure out the number
       of items that were stored in the checkpoint.  Knowing the size, we can restore
       the map from the 2 arrays.  This template only works for string map keys.
     */
    in_map.clear() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_keys")).c_str()) ;
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_data")).c_str()) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        keys = (char **)keys_ref->address ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_map.insert( std::pair<std::string, std::string>(std::string(keys[ii]) , std::string(items[ii]))) ;
        }

        delete_stl( in_map , object_name , var_name ) ;
    }
    return 0 ;
}

int restore_stl(std::map<std::string, std::string> & in_map , std::string object_name , std::string var_name ) ;
int restore_stl(std::multimap<std::string, std::string> & in_map , std::string object_name , std::string var_name ) ;

#endif

#endif
