
#ifndef _TRICK_QUEUE_HH_
#define _TRICK_QUEUE_HH_

#include <string>
#include <queue>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_sequence_stl.hh"

/*
   This algorithm depends on the queue container type is called "c" and is
   a protected variable.  There are no guarantees this works with anything but
   the current gcc versions we are using.
*/
namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Tp, typename _Sequence = std::deque<_Tp>, class dummy = void >
class queue {} ;

// Template use for everything except strings and other STL types.
template <typename _Tp , typename _Sequence >
class queue<_Tp, _Sequence,
 typename std::enable_if< !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::queue<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit queue(const _Sequence& __c = _Sequence()) : std::queue<_Tp, _Sequence>(__c) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized queue for strings
template <typename _Tp , typename _Sequence >
class queue<_Tp, _Sequence, typename std::enable_if<std::is_same<_Tp,std::string>::value>::type >
 : public std::queue<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit queue(const _Sequence& __c = _Sequence()) : std::queue<_Tp, _Sequence>(__c) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_string( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_string( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized queue for other Trick STL types.
template <typename _Tp , typename _Sequence >
class queue<_Tp, _Sequence, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Tp>::value>::type >
 : public std::queue<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit queue(const _Sequence& __s = _Sequence()) : std::queue<_Tp, _Sequence>(__s) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_STLInterface( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_STLInterface( std::queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

/* PRIORITY QUEUE */

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Tp, typename _Sequence = std::vector<_Tp>,
          typename _Compare  = std::less<typename _Sequence::value_type> , class dummy = void >
class priority_queue {} ;

// Template use for everything except strings and other STL types.
template <typename _Tp , typename _Sequence , typename _Compare >
class priority_queue<_Tp, _Sequence, _Compare,
 typename std::enable_if< !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::priority_queue<_Tp, _Sequence, _Compare> , public STLInterface {
    public:
        // default constructor
        explicit priority_queue(const _Compare& __x = _Compare(), const _Sequence& __s = _Sequence())
         : std::priority_queue<_Tp, _Sequence, _Compare>(__x, __s) { }

        // iterator constructor
        template<typename _InputIterator>
         priority_queue(_InputIterator __first, _InputIterator __last,
                      const _Compare& __x = _Compare(),
                      const _Sequence& __s = _Sequence())
         : std::priority_queue<_Tp, _Sequence, _Compare>(__first, __last, __x, __s) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized priority_queue for strings
template <typename _Tp , typename _Sequence , typename _Compare >
class priority_queue<_Tp, _Sequence, _Compare, typename std::enable_if<std::is_same<_Tp,std::string>::value>::type >
 : public std::priority_queue<_Tp, _Sequence, _Compare> , public STLInterface {
    public:
        // default constructor
        explicit priority_queue(const _Compare& __x = _Compare(), const _Sequence& __s = _Sequence())
         : std::priority_queue<_Tp, _Sequence, _Compare>(__x, __s) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_string( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_string( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized priority_queue for other Trick STL types.
template <typename _Tp , typename _Sequence , typename _Compare >
class priority_queue<_Tp, _Sequence, _Compare, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Tp>::value>::type >
 : public std::priority_queue<_Tp, _Sequence, _Compare> , public STLInterface {
    public:
        // default constructor
        explicit priority_queue(const _Compare& __x = _Compare(), const _Sequence& __s = _Sequence())
         : std::priority_queue<_Tp, _Sequence, _Compare>(__x, __s) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_STLInterface( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_STLInterface( std::priority_queue<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

} ;

#endif
