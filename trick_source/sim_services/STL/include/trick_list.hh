
#ifndef _TRICK_LIST_HH_
#define _TRICK_LIST_HH_

#include <string>
#include <list>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_sequence_stl.hh"

namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Tp, typename _Alloc = std::allocator<_Tp> , class dummy = void >
class list {} ;

// Template use for everything except strings and other STL types.
template <typename _Tp , typename _Alloc >
class list<_Tp, _Alloc,
 typename std::enable_if< !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::list<_Tp,_Alloc> , public STLInterface {
    public:
        // default constructor
        list() {} ;

        // fill constructor
        explicit list(size_t __n, const _Tp & __value = _Tp(),
            const _Alloc & __a = _Alloc()) : std::list<_Tp,_Alloc>(__n, __value, __a) {}

        // iterator range constructor
        template<typename _InputIterator>
         list(_InputIterator __first, _InputIterator __last,
              const _Alloc& __a = _Alloc()) : std::list<_Tp,_Alloc>(__first, __last, __a) {}

        // copy constructor
        list(const list & __x) : std::list<_Tp,_Alloc>(__x) {}

        // assignment operator
        list& operator=(const list & __x) {
            std::list<_Tp,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized list for other Trick STL types.
template <typename _Tp , typename _Alloc >
class list<_Tp, _Alloc, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Tp>::value>::type > : public std::list<_Tp,_Alloc> , public STLInterface {
    public:
        // default constructor
        list() {} ;

        // fill constructor
        explicit list(size_t __n, const _Tp & __value = _Tp(),
            const _Alloc & __a = _Alloc()) : std::list<_Tp,_Alloc>(__n, __value, __a) {}

        // iterator range constructor
        template<typename _InputIterator>
         list(_InputIterator __first, _InputIterator __last,
              const _Alloc& __a = _Alloc()) : std::list<_Tp,_Alloc>(__first, __last, __a) {}

        // copy constructor
        list(const list & __x) : std::list<_Tp,_Alloc>(__x) {}

        // assignment operator
        list& operator=(const list & __x) {
            std::list<_Tp,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_STLInterface( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_STLInterface( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized list for strings
template <typename _Tp , typename _Alloc >
class list<_Tp, _Alloc, typename std::enable_if<std::is_same<_Tp,std::string>::value>::type > : public std::list<_Tp,_Alloc> , public STLInterface {
    public:
        // default constructor
        list() {} ;

        // fill constructor
        explicit list(size_t __n, const _Tp & __value = _Tp(),
            const _Alloc & __a = _Alloc()) : std::list<_Tp,_Alloc>(__n, __value, __a) {}

        // iterator range constructor
        template<typename _InputIterator>
         list(_InputIterator __first, _InputIterator __last,
              const _Alloc& __a = _Alloc()) : std::list<_Tp,_Alloc>(__first, __last, __a) {}

        // copy constructor
        list(const list & __x) : std::list<_Tp,_Alloc>(__x) {}

        // assignment operator
        list& operator=(const list & __x) {
            std::list<_Tp,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_sequence_stl_string( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_sequence_stl_string( *this , obj_name , stl_id ) ;
        }
} ;

} ;

#endif
