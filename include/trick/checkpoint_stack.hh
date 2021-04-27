
/*
    PURPOSE: (Helpers to checkpoint STLs)
*/

#ifndef CHECKPOINT_STACK_HH
#define CHECKPOINT_STACK_HH

#include <stack>
#include <algorithm>
#include <typeinfo>
#include <sstream>
#include <type_traits>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "trick/checkpoint_sequence_stl.hh"
#include "checkpoint_stl_protos.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"

/* =================================================================================================*/

// stack: intrinsic
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::stack<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    std::string temp_str ;
    int status ;

    ITEM_TYPE * items = nullptr ;
    std::stack<ITEM_TYPE,_Sequence> temp_stack(in_stl) ;

    cont_size = temp_stack.size() ;
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
        items = (ITEM_TYPE *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( items ) {
            temp_str = std::string(object_name + "_" + var_name) ;
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                items[ii] = temp_stack.top() ;
                temp_stack.pop() ;
            }
        }
    }

    return 0 ;
}

// stack: STL
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int checkpoint_stl(std::stack<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    std::string temp_str ;

    std::string * items = nullptr ;
    std::stack<ITEM_TYPE,_Sequence> temp_stack(in_stl) ;

    cont_size = temp_stack.size() ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    if ( cont_size > 0 ) {
        var_declare << "std::string "
         << object_name << "_" << var_name << "[" << cont_size << "]" ;
        temp_str = var_declare.str() ;
        items = (std::string *)TMM_declare_var_s(temp_str.c_str()) ;
        if ( items ) {
            temp_str = std::string(object_name + "_" + var_name) ;
            TMM_add_checkpoint_alloc_dependency(temp_str.c_str()) ;
            //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

            for ( ii = 0 ; ii < cont_size ; ii++ ) {
                std::ostringstream sub_elements ;
                sub_elements << object_name << "_" << var_name << "_" << ii ;
                items[ii] = sub_elements.str() ;

                std::ostringstream index_string ;
                index_string << ii ;
                checkpoint_stl (temp_stack.top(), object_name + "_" + var_name, index_string.str()) ;
                temp_stack.pop() ;
            }
        }
    }

    return 0 ;
}

/* =================================================================================================*/

// The delete routine uses the same method as the sequence types

template <typename ITEM_TYPE, typename _Sequence>
int delete_stl(std::stack<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    return delete_sequence_alloc( in_stl , object_name , var_name ) ;
}

/* =================================================================================================*/

/* Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number of
   items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */
template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::stack<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;
    std::string temp_str ;

    REF2 * items_ref ;
    ITEM_TYPE * items = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_STACK %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + std::string("_") + var_name ;
    items_ref = ref_attributes(temp_str.c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }
        items = (ITEM_TYPE *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = cont_size - 1 ; ii < cont_size ; ii-- ) {
            in_stl.push( items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

template <typename ITEM_TYPE, typename _Sequence,
          typename std::enable_if< is_stl_container<ITEM_TYPE>::value>::type* >
int restore_stl(std::stack<ITEM_TYPE,_Sequence> & in_stl , std::string object_name , std::string var_name ) {
    unsigned int ii ;
    unsigned int cont_size ;
    std::string temp_str ;

    REF2 * items_ref ;
    std::string * items = nullptr ;
    std::replace_if(object_name.begin(), object_name.end(), static_cast<int (*)(int)>(std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_STACK %s_%s\n", object_name.c_str() , var_name.c_str()) ;
    temp_str = object_name + std::string("_") + var_name ;
    items_ref = ref_attributes(temp_str.c_str()) ;

    if ( items_ref != NULL ) {
        cont_size = in_stl.size() ;
        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            in_stl.pop() ;
        }
        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = cont_size - 1 ; ii < cont_size ; ii-- ) {
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
