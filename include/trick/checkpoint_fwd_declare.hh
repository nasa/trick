/*
    PURPOSE: (Foward declare all the checkpoint_stl routines that are used in other template routines.)
*/

#ifndef CHECKPOINT_FWD_DECLARE
#define CHECKPOINT_FWD_DECLARE

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <utility>

#include <string>
#include <type_traits>

#include "checkpoint_is_stl_container.hh"

// This file declares all of the variants of the checkpoint_stl function.
// Many of the checkpoint_stl templates are recursive, calling cechkpoint_stl again with their
// template argument type.  This header makes all of the variants available
// to the recursive templates.

// Checkpoint routines

// vector
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// list
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// deque
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// set
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// multiset
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int checkpoint_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// map
template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

// multimap
template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int checkpoint_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

// pair
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* = nullptr >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* = nullptr >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* = nullptr >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* = nullptr >
int checkpoint_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

/* ===================================================================================================== */

// Restore routines

// vector
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::vector<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// list
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::list<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// deque
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::deque<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// set
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::set<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// multiset
template <typename ITEM_TYPE, typename std::enable_if<is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

template <typename ITEM_TYPE, typename std::enable_if<!is_stl_container<ITEM_TYPE>::value>::type* = nullptr >
int restore_stl(std::multiset<ITEM_TYPE> & in_stl , std::string object_name , std::string var_name ) ;

// map
template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::map<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

// multimap
template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if<!is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                         !is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

template <class KEY, class DATA, typename std::enable_if< is_stl_container<KEY>::value &&
                                                          is_stl_container<DATA>::value >::type* = nullptr >
int restore_stl(std::multimap<KEY , DATA> & in_map , std::string object_name , std::string var_name ) ;

// pair
template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* = nullptr >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if<!is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* = nullptr >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                             !is_stl_container<SECOND>::value >::type* = nullptr >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

template <class FIRST, class SECOND, typename std::enable_if< is_stl_container<FIRST>::value &&
                                                              is_stl_container<SECOND>::value >::type* = nullptr >
int restore_stl(std::pair<FIRST , SECOND> & in_pair , std::string object_name , std::string var_name ) ;

#endif

