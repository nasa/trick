
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_SEQUENCE_STL_HH
#define CHECKPOINT_SEQUENCE_STL_HH

#include <string>
#include <array>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <algorithm>
#include <typeinfo>
#include <sstream>
#include <type_traits>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "checkpoint_is_stl_container.hh"
#include "checkpoint_stl_protos.hh"
#include "checkpoint_fwd_declare.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"

/* =================================================================================================*/

template <class STL>
int checkpoint_sequence_i(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    std::string temp_str; 
    int status ;

    typename STL::value_type * items = NULL ;
    typename STL::iterator it ;
    typename STL::iterator end ;

    //message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    if ( cont_size > 0 ) {
        std::string type_string;
        try {
            type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status )) ;
        } catch (const std::bad_typeid& e) {
            message_publish(1, "Error, having difficulty checkpointing %s.%s\n", object_name.c_str(), var_name.c_str()) ;
            return 0 ;
        }
        var_declare << type_string << " "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        temp_str = var_declare.str() ;
        items = (typename STL::value_type *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( items ) {
            temp_str = std::string(object_name + "_" + var_name);
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
            //message_publish(1, "CHECKPOINT_SEQUENCE_STL with %s\n", var_declare) ;

            /* copy the contents of the stl */
            for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
                items[ii] = *it ;
            }
        }

    }

    return 0 ;
}

template <class STL>
int checkpoint_sequence_s(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    std::string temp_str ;

    std::string * items ;
    typename STL::iterator it ;
    typename STL::iterator end ;


    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;

    if ( cont_size > 0 ) {

        var_declare << "std::string " << object_name << "_" << var_name << "[" << cont_size << "]" ;
        temp_str = var_declare.str() ;
        items = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( items ) {
            temp_str = std::string(object_name + "_" + var_name) ;
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
            //message_publish(1, "CHECKPOINT_SEQUENCE_STL_STL with %s\n", temp_str.c_str()) ;

            /* create the names of the sub stl checkpoint names we're going to be using */
            for ( ii = 0 , it = in_stl.begin() , end = in_stl.end() ; it != end ; it++ , ii++ ) {
                std::ostringstream sub_elements ;
                sub_elements << object_name << "_" << var_name << "_" << ii ;
                items[ii] = sub_elements.str() ;

                std::ostringstream index_string ;
                index_string << ii ;
                //message_publish(1, "recursive call to checkpoint_stl %s\n", __PRETTY_FUNCTION__) ;
                checkpoint_stl( (*it) , object_name + "_" + var_name , index_string.str() ) ;
            }
        }
    }
    return 0 ;
}

// -----------
// std::array

// This template is only enabled if the items in the array are an STL
template <typename ITEM_TYPE, std::size_t N,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::array<ITEM_TYPE,N> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the array are NOT an STL, except for std::string
template <typename ITEM_TYPE, std::size_t N,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::array<ITEM_TYPE,N> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::vector

// This template is only enabled if the items in the vector are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::vector<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the vector are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::vector<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::list

template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::list<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the list are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::list<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::deque

template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::deque<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the deque are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::deque<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::set

template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::set<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the set are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::set<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::multiset

template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::multiset<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the multiset are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::multiset<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return checkpoint_sequence_i( in_stl , object_name , var_name ) ;
}

/* =================================================================================================*/

template <class STL>
int delete_sequence_alloc(STL & in_stl __attribute__ ((unused)), std::string object_name , std::string var_name ) {
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');
    REF2 * items_ref ;
    std::string temp_str = object_name + std::string("_") + var_name ;
    items_ref = ref_attributes((char *)temp_str.c_str()) ;
    if ( items_ref != NULL ) {
        temp_str = object_name + std::string("_") + var_name ;
        TMM_delete_var_n(temp_str.c_str() ) ;
        free(items_ref) ;
    }
    return 0 ;
}

template <class ITEM_TYPE, std::size_t N>
int delete_stl(std::array<ITEM_TYPE,N> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE, typename _Alloc>
int delete_stl(std::vector<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE, typename _Alloc>
int delete_stl(std::list<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE, typename _Alloc>
int delete_stl(std::deque<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE, typename _Alloc>
int delete_stl(std::set<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <class ITEM_TYPE, typename _Alloc>
int delete_stl(std::multiset<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}


/* =================================================================================================*/

/* Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */
template <class STL>
int restore_sequence_i(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::string temp_str ;

    REF2 * items_ref ;
    typename STL::value_type * items ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + std::string("_") + var_name ;
    items_ref = ref_attributes((char *)temp_str.c_str()) ;

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
int restore_sequence_s(STL & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    std::string * items ;
    std::string temp_str ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;
    temp_str = object_name + "_" + var_name ;
    items_ref = ref_attributes((char *)temp_str.c_str()) ;

    if ( items_ref != NULL ) {
        in_stl.clear() ;
        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename STL::value_type vt ;
            std::ostringstream index_string ;
            index_string << ii ;
            restore_stl(vt, object_name + "_" + var_name , index_string.str()) ;
            in_stl.insert( in_stl.end(), vt ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

// -----------
// std::array

// This template is only enabled if the items in the array are an STL
template <typename ITEM_TYPE, std::size_t N,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::array<ITEM_TYPE,N> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    std::string * items ;
    std::string temp_str ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "%s\n", __PRETTY_FUNCTION__) ;
    temp_str = object_name + "_" + var_name ;
    items_ref = ref_attributes((char *)temp_str.c_str()) ;

    if ( items_ref != NULL ) {
        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            typename std::array<ITEM_TYPE,N>::value_type vt ;
            std::ostringstream index_string ;
            index_string << ii ;
            restore_stl(vt, object_name + "_" + var_name , index_string.str()) ;
            in_stl[ii] = vt ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

// This template is only enabled if the items in the array are NOT an STL, except for std::string
template <typename ITEM_TYPE, std::size_t N,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::array<ITEM_TYPE,N> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;
    std::string temp_str ;

    REF2 * items_ref ;
    typename std::array<ITEM_TYPE,N>::value_type * items ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "RESTORE_SEQUENCE_STL %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + std::string("_") + var_name ;
    items_ref = ref_attributes((char *)temp_str.c_str()) ;

    if ( items_ref != NULL ) {
        items = (typename std::array<ITEM_TYPE,N>::value_type *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl[ii] = items[ii] ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

// -----------
// std::vector

// This template is only enabled if the items in the vector are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::vector<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the vector are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::vector<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::list

// This template is only enabled if the items in the list are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::list<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the list are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::list<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::deque

// This template is only enabled if the items in the deque are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::deque<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the deque are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::deque<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::set

// This template is only enabled if the items in the set are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::set<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the set are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::set<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_i( in_stl , object_name , var_name ) ;
}

// -----------
// std::multiset

// This template is only enabled if the items in the multiset are an STL
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::multiset<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_s( in_stl , object_name , var_name ) ;
}

// This template is only enabled if the items in the multiset are NOT an STL, except for std::string
template <typename ITEM_TYPE, typename _Alloc,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::multiset<ITEM_TYPE,_Alloc> & in_stl , std::string object_name , std::string var_name ) {
    return restore_sequence_i( in_stl , object_name , var_name ) ;
}

#endif
