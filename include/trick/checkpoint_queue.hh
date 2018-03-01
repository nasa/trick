
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_QUEUE_HH
#define CHECKPOINT_QUEUE_HH

#include <queue>
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
#include "checkpoint_sequence_stl.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"

/* =================================================================================================*/


// queue: intrinsic
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::queue<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    int status ;

    ITEM_TYPE * items = nullptr ;
    std::queue<ITEM_TYPE,_Sequence> temp_queue(in_stl) ;

    cont_size = temp_queue.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status )) ;
        var_declare << type_string << " "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( items ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                items[ii] = temp_queue.front() ;
                temp_queue.pop() ;
            }
        }
    }

    return 0 ;
}

// queue: STL
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::queue<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;

    std::string * items = nullptr ;
    std::queue<ITEM_TYPE,_Sequence> temp_queue(in_stl) ;

    cont_size = temp_queue.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        var_declare << "std::string "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        items = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( items ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                std::ostringstream sub_elements ;
                sub_elements << object_name << "_" << var_name << "_" << ii ;
                items[ii] = sub_elements.str() ;

                std::ostringstream index_string ;
                index_string << ii ;
                checkpoint_stl (temp_queue.front(), object_name + "_" + var_name, index_string.str()) ;
                temp_queue.pop() ;
            }
        }
    }
    return 0 ;
}

// priority queue: intrinsic
template <typename ITEM_TYPE, typename _Container, typename _Compare,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::priority_queue<ITEM_TYPE, _Container, _Compare> & in_stl ,
                   std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    int status ;

    ITEM_TYPE * items = nullptr ;
    std::priority_queue<ITEM_TYPE,_Container,_Compare> temp_queue(in_stl) ;

    cont_size = temp_queue.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status )) ;
        var_declare << type_string << " "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        items = (ITEM_TYPE *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( items ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                items[ii] = temp_queue.top() ;
                temp_queue.pop() ;
            }
        }
    }

    return 0 ;
}

// priority queue: STL
template <typename ITEM_TYPE, typename _Container, typename _Compare,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::priority_queue<ITEM_TYPE, _Container, _Compare> & in_stl ,
                   std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;

    std::string * items = nullptr ;
    std::priority_queue<ITEM_TYPE,_Container,_Compare> temp_queue(in_stl) ;

    cont_size = temp_queue.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        var_declare << "std::string "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        items = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( items ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name).c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                std::ostringstream sub_elements ;
                sub_elements << object_name << "_" << var_name << "_" << ii ;
                items[ii] = sub_elements.str() ;

                std::ostringstream index_string ;
                index_string << ii ;
                checkpoint_stl (const_cast< ITEM_TYPE &>(temp_queue.top()), object_name + "_" + var_name, index_string.str()) ;
                temp_queue.pop() ;
            }
        }
    }

    return 0 ;
}

/* =================================================================================================*/

// The delete routines use the same method as the sequence types

template <typename ITEM_TYPE, typename _Sequence>
int delete_stl(std::queue<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

template <typename ITEM_TYPE, typename _Container, typename _Compare>
int delete_stl(std::priority_queue<ITEM_TYPE,_Container,_Compare> & in_stl ,
                    std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

/* =================================================================================================*/

/* Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */

// queue: intrinsic
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::queue<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    ITEM_TYPE * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }

        items = (ITEM_TYPE *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.push( items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }
    return 0 ;
}

// queue: STL
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::queue<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    std::string * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    items_ref = ref_attributes((char *)(object_name + "_" + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }

        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            ITEM_TYPE vt ;
            std::ostringstream index_string ;
            index_string << ii ;
            restore_stl( vt , object_name + "_" + var_name , index_string.str()) ;
            in_stl.push( vt ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }
    return 0 ;
}

// priority_queue: intrinsic
template <typename ITEM_TYPE, typename _Container, typename _Compare,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::priority_queue<ITEM_TYPE,_Container,_Compare> & in_stl ,
                    std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    ITEM_TYPE * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }

        items = (ITEM_TYPE *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.push( items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }
    return 0 ;
}

// priority_queue: STL
template <typename ITEM_TYPE, typename _Container, typename _Compare,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::priority_queue<ITEM_TYPE,_Container,_Compare> & in_stl ,
                    std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    std::string * items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    items_ref = ref_attributes((char *)(object_name + "_" + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }

        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            ITEM_TYPE vt ;
            std::ostringstream index_string ;
            index_string << ii ;
            restore_stl( vt , object_name + "_" + var_name , index_string.str()) ;
            in_stl.push( vt ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }
    return 0 ;
}

#endif
