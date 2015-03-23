
#ifndef TRICK_STACK_HH
#define TRICK_STACK_HH

#include <string>
#include <stack>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_sequence_stl.hh"

/*
   This algorithm depends on the stack container type is called "c" and is
   a protected variable.  There are no guarantees this works with anything but
   the current gcc versions we are using.
*/
namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Tp, typename _Sequence = std::deque<_Tp>, class dummy = void >
class stack {} ;

// Template use for everything except strings and other STL types.
template <typename _Tp , typename _Sequence >
class stack<_Tp, _Sequence, typename std::enable_if< !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::stack<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit stack(const _Sequence& __c = _Sequence()) : std::stack<_Tp, _Sequence>(__c) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized stack for strings
template <typename _Tp , typename _Sequence >
class stack<_Tp, _Sequence, typename std::enable_if<std::is_same<_Tp,std::string>::value>::type >
 : public std::stack<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit stack(const _Sequence& __c = _Sequence()) : std::stack<_Tp, _Sequence>(__c) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_string( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_string( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

// Specialized stack for other Trick STL types.
template <typename _Tp, typename _Sequence >
class stack<_Tp, _Sequence, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Tp>::value>::type >
 : public std::stack<_Tp, _Sequence> , public STLInterface {
    public:
        // default constructor
        explicit stack(const _Sequence& __c = _Sequence()) : std::stack<_Tp, _Sequence>(__c) { }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_STLInterface( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_STLInterface( std::stack<_Tp, _Sequence>::c , obj_name , stl_id ) ;
        }
} ;

} ;

#endif
