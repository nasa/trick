
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_SEQUENCE_STL_HH
#define CHECKPOINT_SEQUENCE_STL_HH

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <algorithm>
#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"

#ifndef TRICK_ICG

template <class STL>
int delete_sequence_stl(STL & in_stl __attribute__ ((unused)), std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    REF2 * items_ref ;
    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n((object_name + std::string("_") + var_name).c_str() ) ;
        free(items_ref) ;
    }
    return 0 ;
}

template <class ITEM_TYPE>
int delete_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int delete_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class STL>
int checkpoint_sequence_stl(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    typename STL::value_type * items = NULL ;
    typename STL::iterator it ;
    typename STL::iterator end ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
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
int checkpoint_sequence_stl_string(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    const char ** items ;
    typename STL::iterator it ;
    typename STL::iterator end ;


    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "char * %s_%s[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        items = (const char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL_STRING with %s\n", var_declare) ; 

        /* copy the contents of the vector */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            items[ii] = it->c_str() ;
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
int restore_sequence_stl(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    typename STL::value_type * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    in_stl.clear() ; 

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ; 

    if ( items_ref != NULL ) {
        items = (typename STL::value_type *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.insert( in_stl.end(), items[ii] ) ;
        } 
        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

template <class STL>
int restore_sequence_stl_string(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    char ** items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "in specialized vector template restore\n") ;

    in_stl.clear() ;

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.insert( in_stl.end(), items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

// General routines for all types but strings
template <class ITEM_TYPE>
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE>
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl( in_stl , object_name , var_name ) ;
}
// Specialized routines for strings.
int checkpoint_stl(std::vector<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::list<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::deque<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::set<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int checkpoint_stl(std::multiset<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int restore_stl(std::vector<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int restore_stl(std::list<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int restore_stl(std::deque<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int restore_stl(std::set<std::string> & in_vector , std::string object_name , std::string var_name ) ;
int restore_stl(std::multiset<std::string> & in_vector , std::string object_name , std::string var_name ) ;
#endif

#endif
