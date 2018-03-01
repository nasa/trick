/*
    PURPOSE: (Illustrate how to checkpoint STLs)
    LIBRARY_DEPENDENCIES: (
      (checkpoint_map.o)
    )
*/

#ifndef CHECKPOINT_MAP_HH
#define CHECKPOINT_MAP_HH

#include <map>
#include <string>
#include <algorithm>
#include <sstream>
#include <typeinfo>
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

/* For all of the checkpoint_map_stl variations.  Declare 2 memory manager arrays to hold
   the contents of the map. No simulation variable actually points to these arrays.  Only the
   user knows they are there and how they are stored.  The restart function knows how to
   "retrieve" and restore the data.  The name of the declaration is important.  Each instance of
   the STL requires a unique name to be saved correctly.  The incoming "object_name" and "var_name"
   gives that unique name.
 */

// intrinsic key, intrinsic data
template <class STL>
int checkpoint_map_ik_id(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys = nullptr ;
    typename STL::mapped_type * items = nullptr ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status )) ;
        var_declare << type_string << " "
         << object_name << "_" << var_name << "_keys[" << cont_size << "]" ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( keys ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_keys").c_str()) ;
            //message_publish(1, "HERE with %s\n", var_declare) ;

            var_declare.str("") ;
            var_declare.clear() ;
            type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status )) ;
            var_declare << type_string << " "
             << object_name << "_" << var_name << "_data[" << cont_size << "]" ;
            items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare.str().c_str()) ;
            if ( items ) {
                TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_data").c_str()) ;
                //message_publish(1, "HERE with %s\n", var_declare) ;

                /* copy the contents of the map the 2 arrays */
                for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
                    keys[ii] = iter->first ;
                    items[ii] = iter->second ;
                }
            }
        }
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int checkpoint_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_ik_id( in_map , object_name , var_name ) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type*>
int checkpoint_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_ik_id( in_map , object_name , var_name ) ;
}

// intrinsic key, STL data
template <class STL>
int checkpoint_map_ik_sd(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    typename STL::iterator iter ;
    int status ;

    typename STL::key_type * keys = nullptr ;
    std::string * items = nullptr ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*keys).name(), 0, 0, &status )) ;
        var_declare << type_string << " "
         << object_name << "_" << var_name << "_keys[" << cont_size << "]" ;
        keys = (typename STL::key_type *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( keys ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_keys").c_str()) ;
            //message_publish(1, "HERE with %s\n", var_declare) ;

            var_declare.str("") ;
            var_declare.clear() ;
            var_declare << "std::string "
             << object_name << "_" << var_name << "_data[" << cont_size << "]" ;
            items = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
            if ( items ) {
                TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_data").c_str()) ;
                //message_publish(1, "HERE with %s\n", var_declare) ;

                /* copy the contents of the map the 2 arrays */
                for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
                    keys[ii] = iter->first ;

                    std::ostringstream sub_elements ;
                    sub_elements << object_name << "_" << var_name << "_data_" << ii ;
                    items[ii] = sub_elements.str() ;

                    std::ostringstream index_string ;
                    index_string << ii ;
                    //message_publish(1, "recursive call to checkpoint_stl %s\n", __PRETTY_FUNCTION__) ;
                    checkpoint_stl( iter->second , object_name + "_" + var_name + "_data" , index_string.str() ) ;
                }
            }
        }
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int checkpoint_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_ik_sd( in_map , object_name , var_name ) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type*>
int checkpoint_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_ik_sd( in_map , object_name , var_name ) ;
}

// STL key, intrinsic data
template <class STL>
int checkpoint_map_sk_id(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    typename STL::iterator iter ;
    int status ;

    std::string * keys = nullptr ;
    typename STL::mapped_type * items = nullptr ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        var_declare << "std::string "
         << object_name << "_" << var_name << "_keys[" << cont_size << "]" ;
        keys = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( keys ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_keys").c_str()) ;
            //message_publish(1, "HERE with %s\n", var_declare) ;

            var_declare.str("") ;
            var_declare.clear() ;
            std::string type_string = stl_type_name_convert(abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status )) ;
            var_declare << type_string << " "
             << object_name << "_" << var_name << "_data[" << cont_size << "]" ;
            items = (typename STL::mapped_type *)TMM_declare_var_s(var_declare.str().c_str()) ;
            if ( items ) {
                TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_data").c_str()) ;
                //message_publish(1, "HERE with %s\n", var_declare) ;

                /* copy the contents of the map the 2 arrays */
                for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
                    std::ostringstream sub_elements ;
                    sub_elements << object_name << "_" << var_name << "_keys_" << ii ;
                    keys[ii] = sub_elements.str() ;

                    std::ostringstream index_string ;
                    index_string << ii ;
                    checkpoint_stl( const_cast<typename STL::key_type &>(iter->first) ,
                     object_name + "_" + var_name + "_keys", index_string.str() ) ;

                    items[ii] = iter->second ;
                }
            }
        }
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int checkpoint_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_sk_id( in_map , object_name , var_name ) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type*>
int checkpoint_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_sk_id( in_map , object_name , var_name ) ;
}

// STL key, STL data
template <class STL>
int checkpoint_map_stl_sk_sd(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    std::ostringstream var_declare ;
    typename STL::iterator iter ;

    std::string * keys = nullptr ;
    std::string * items = nullptr ;

    cont_size = in_map.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {
        var_declare << "std::string "
         << object_name << "_" << var_name << "_keys[" << cont_size << "]" ;
        keys = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
        if ( keys ) {
            TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_keys").c_str()) ;
            //message_publish(1, "HERE with %s\n", var_declare) ;

            var_declare.str("") ;
            var_declare.clear() ;
            var_declare << "std::string "
             << object_name << "_" << var_name << "_data[" << cont_size << "]" ;
            items = (std::string *)TMM_declare_var_s(var_declare.str().c_str()) ;
            if ( items ) {
                TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_data").c_str()) ;
                //message_publish(1, "HERE with %s\n", var_declare) ;

                /* copy the contents of the map the 2 arrays */
                for ( iter = in_map.begin() , ii = 0 ; iter != in_map.end() ; iter++ , ii++ ) {
                    std::ostringstream sub_elements ;
                    sub_elements << object_name << "_" << var_name << "_keys_" << ii ;
                    keys[ii] = sub_elements.str() ;

                    std::ostringstream index_string ;
                    index_string << ii ;
                    checkpoint_stl( const_cast<typename STL::key_type &>(iter->first) ,
                     object_name + "_" + var_name + "_keys", index_string.str() ) ;

                    sub_elements << object_name << "_" << var_name << "_data_" << ii ;
                    items[ii] = sub_elements.str() ;

                    checkpoint_stl( iter->second ,
                     object_name + "_" + var_name + "_data", index_string.str() ) ;
                }
            }
        }
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int checkpoint_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_sk_sd( in_map , object_name , var_name ) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type*>
int checkpoint_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_sk_sd( in_map , object_name , var_name ) ;
}
/* =================================================================================================*/

template <class STL>
int delete_map_allocs(STL & in_map __attribute__ ((unused)), std::string object_name , std::string var_name ) {
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

template <class KEY, class DATA, typename _Compare, typename _Alloc>
int delete_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return delete_map_allocs(in_map , object_name , var_name) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc>
int delete_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return delete_map_allocs(in_map , object_name , var_name) ;
}

/* =================================================================================================*/

/* For all restore_map_stl varieties.
   Find the arrays the map data was stored in the checkpoint using ref_attributes
   From the address of the resulting ref_attributes, we can figure out the number
   of items that were stored in the checkpoint.  Knowing the size, we can restore
   the map from the 2 arrays.
 */
template <class STL>
int restore_map_ik_id(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    typename STL::mapped_type * items ;

    //message_publish(1, "in regular map template restore\n") ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + "_" + var_name + "_keys").c_str()) ;
    items_ref = ref_attributes((char *)(object_name + "_" + var_name + "_data").c_str()) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        in_map.clear() ;
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

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int restore_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_ik_id(in_map , object_name , var_name) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int restore_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_ik_id(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_ik_sd(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    typename STL::key_type * keys ;
    std::string * items ;

    //message_publish(1, "in regular map template restore\n") ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + "_" + var_name + "_keys").c_str()) ;
    items_ref = ref_attributes((char *)(object_name + "_" + var_name + "_data").c_str()) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        in_map.clear() ;
        keys = (typename STL::key_type *)keys_ref->address ;
        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            std::ostringstream index_string ;
            index_string << ii ;

            typename STL::mapped_type mt ;
            restore_stl(mt, object_name + "_" + var_name + "_data", index_string.str()) ;
            in_map.insert( std::pair<typename STL::key_type , typename STL::mapped_type>(keys[ii], mt)) ;
        }
        delete_stl( in_map , object_name , var_name ) ;
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int restore_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_ik_sd(in_map , object_name , var_name) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if<!is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int restore_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_ik_sd(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_sk_id(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    std::string * keys ;
    typename STL::mapped_type * items ;

    //message_publish(1, "in regular map template restore\n") ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + "_" + var_name + "_keys").c_str()) ;
    items_ref = ref_attributes((char *)(object_name + "_" + var_name + "_data").c_str()) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        in_map.clear() ;
        keys = (std::string *)keys_ref->address ;
        items = (typename STL::mapped_type *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            std::ostringstream index_string ;
            index_string << ii ;

            typename STL::key_type kt ;
            restore_stl(kt, object_name + "_" + var_name + "_keys", index_string.str()) ;
            in_map.insert( std::pair<typename STL::key_type , typename STL::mapped_type>(kt, items[ii])) ;
        }
        delete_stl( in_map , object_name , var_name ) ;
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int restore_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_sk_id(in_map , object_name , var_name) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                  !is_stl_container<DATA>::value >::type* >
int restore_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_sk_id(in_map , object_name , var_name) ;
}

template <class STL>
int restore_map_sk_sd(STL & in_map , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * keys_ref , * items_ref ;
    std::string * keys ;
    std::string * items ;

    //message_publish(1, "in regular map template restore\n") ;

    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    keys_ref = ref_attributes((char *)(object_name + "_" + var_name + "_keys").c_str()) ;
    items_ref = ref_attributes((char *)(object_name + "_" + var_name + "_data").c_str()) ;

    if ( keys_ref != NULL && items_ref != NULL ) {
        in_map.clear() ;
        keys = (std::string *)keys_ref->address ;
        items = (std::string *)items_ref->address ;
        cont_size = get_size((char *)keys) ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            std::ostringstream index_string ;
            index_string << ii ;

            typename STL::key_type kt ;
            restore_stl(kt, object_name + "_" + var_name + "_keys", index_string.str()) ;

            typename STL::mapped_type mt ;
            restore_stl(mt, object_name + "_" + var_name + "_data", index_string.str()) ;

            in_map.insert( std::pair<typename STL::key_type , typename STL::mapped_type>(kt, mt)) ;
        }
        delete_stl( in_map , object_name , var_name ) ;
    }
    return 0 ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int restore_stl(std::map<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_sk_sd(in_map , object_name , var_name) ;
}

template <class KEY, class DATA, typename _Compare, typename _Alloc,
          typename std::enable_if< is_stl_container<KEY>::value &&
                                   is_stl_container<DATA>::value >::type* >
int restore_stl(std::multimap<KEY,DATA,_Compare,_Alloc> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_sk_sd(in_map , object_name , var_name) ;
}

#endif
