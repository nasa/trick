
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

#include <stack>
#include <queue>
#include <utility>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <type_traits>

#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"
#include "checkpoint_is_stl_container.hh"

/* =================================================================================================*/

// Forward declare all of the specialized templates that can be used in checkpoint_sequence_stl_stl
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;
// End forward declarations.

template <class STL>
int checkpoint_sequence_stl_intrinsic(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    typename STL::value_type * items = NULL ;
    typename STL::iterator it ;
    typename STL::iterator end ;

    message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (typename STL::value_type *)TMM_declare_var_s(var_declare) ;
        TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
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
        TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL_STRING with %s\n", var_declare) ;

        /* copy the contents of the vector */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            items[ii] = it->c_str() ;
        }
    }

    return 0 ;
}

template <class STL>
int checkpoint_sequence_stl_stl(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;

    const char ** items ;
    typename STL::iterator it ;
    typename STL::iterator end ;


    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;

    if ( cont_size > 0 ) {

        sprintf(var_declare, "char * %s_%s[%d]" , object_name.c_str(), var_name.c_str() , cont_size) ;
        items = (const char **)TMM_declare_var_s(var_declare) ;
        TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
        //message_publish(1, "CHECKPOINT_SEQUENCE_STL_STL with %s\n", var_declare) ;

        /* create the names of the sub stl checkpoint names we're going to be using */
        for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
            char sub_elements[128] ;
            sprintf(sub_elements, "%s_%s_%d", object_name.c_str(), var_name.c_str() , ii) ;
            items[ii] = TMM_strdup(sub_elements) ;
            sprintf(sub_elements, "%s_%s_%d_name", object_name.c_str(), var_name.c_str() , ii) ;
            set_alloc_name_at((void *)items[ii], sub_elements) ;
            TMM_add_checkpoint_alloc_dependency(sub_elements) ;

            char index_string[16] ;
            sprintf(index_string, "%d", ii) ;
            message_publish(1, "recursive call to checkpoint_stl %s\n", __PRETTY_FUNCTION__) ;
            checkpoint_stl( (*it) , object_name + "_" + var_name , std::string(index_string) ) ;
        }
    }
    return 0 ;
}

// -----------
// std::vector

// This template is only enabled if the items in the vector are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the vector are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings. found in checkpoint_sequence.cpp
int checkpoint_stl(std::vector<std::string> & in_vector , std::string object_name , std::string var_name ) ;

// -----------
// std::list

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the list are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings. found in checkpoint_sequence.cpp
int checkpoint_stl(std::list<std::string> & in_list , std::string object_name , std::string var_name ) ;

// -----------
// std::deque

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the deque are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings. found in checkpoint_sequence.cpp
int checkpoint_stl(std::deque<std::string> & in_vector , std::string object_name , std::string var_name ) ;

// -----------
// std::set

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the set are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings. found in checkpoint_sequence.cpp
int checkpoint_stl(std::set<std::string> & in_vector , std::string object_name , std::string var_name ) ;

// -----------
// std::multiset

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the multiset are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings. found in checkpoint_sequence.cpp
int checkpoint_stl(std::multiset<std::string> & in_vector , std::string object_name , std::string var_name ) ;

/* =================================================================================================*/

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


/* =================================================================================================*/

// Forward declare all of the specialized templates that can be used in checkpoint_sequence_stl_stl
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;
// End forward declarations.

/* Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */
template <class STL>
int restore_sequence_stl_intrinsic(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    typename STL::value_type * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;


    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ; 

    if ( items_ref != NULL ) {
        in_stl.clear() ;
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


    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        in_stl.clear() ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.insert( in_stl.end(), items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

template <class STL>
int restore_sequence_stl_stl(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    char ** items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;

    items_ref = ref_attributes((char *)(object_name + "_" + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        in_stl.clear() ;
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::value_type vt ;
            char index_string[16] ;
            sprintf(index_string, "%d", ii) ;
            restore_stl(vt, object_name + "_" + var_name , std::string(index_string)) ;
            in_stl.insert( in_stl.end(), vt ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

// -----------
// std::vector

// This template is only enabled if the items in the vector are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the vector are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings.
int restore_stl(std::vector<std::string> & in_vector , std::string object_name , std::string var_name ) ;

// -----------
// std::list

// This template is only enabled if the items in the list are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the list are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

int restore_stl(std::list<std::string> & in_list , std::string object_name , std::string var_name ) ;

// -----------
// std::deque

// This template is only enabled if the items in the deque are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the deque are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

int restore_stl(std::deque<std::string> & in_deque , std::string object_name , std::string var_name ) ;

// -----------
// std::set

// This template is only enabled if the items in the set are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the set are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

int restore_stl(std::set<std::string> & in_set , std::string object_name , std::string var_name ) ;

// -----------
// std::multiset

// This template is only enabled if the items in the multiset are an STL
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_stl( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the multiset are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_stl_intrinsic( in_stl , object_name , var_name ) ;
}

// Specialized routines for strings.
int restore_stl(std::multiset<std::string> & in_multiset , std::string object_name , std::string var_name ) ;

#endif
