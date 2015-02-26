
#ifndef _TRICK_SET_HH_
#define _TRICK_SET_HH_

#include <string>
#include <set>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_sequence_stl.hh"

namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Key, typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<_Key>, class dummy = void >
class set {} ;

// Template use for everything except strings and other STL types.
template <typename _Key , typename _Compare , typename _Alloc >
class set<_Key, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value
                        >::type > : public std::set<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        set() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last) : std::set<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::set<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        set(const set & __x) : std::set<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        set& operator=(const set & __x) {
            std::set<_Key,_Compare,_Alloc>::operator = (__x) ;
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

// Specialized set for other Trick STL types.
template <typename _Key , typename _Compare , typename _Alloc >
class set<_Key, _Compare, _Alloc, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Key>::value>::type >
 : public std::set<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        set() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last) : std::set<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::set<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        set(const set & __x) : std::set<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        set& operator=(const set & __x) {
            std::set<_Key,_Compare,_Alloc>::operator = (__x) ;
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

// Specialized set for strings
template <typename _Key , typename _Compare , typename _Alloc >
class set<_Key, _Compare, _Alloc, typename std::enable_if<std::is_same<_Key,std::string>::value>::type >
 : public std::set<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        set() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last) : std::set<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         set(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::set<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        set(const set & __x) : std::set<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        set& operator=(const set & __x) {
            std::set<_Key,_Compare,_Alloc>::operator = (__x) ;
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

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Key, typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<_Key>, class dummy = void >
class multiset {} ;

// Template use for everything except strings and other STL types.
template <typename _Key , typename _Compare , typename _Alloc >
class multiset<_Key, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value
                        >::type > : public std::multiset<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multiset() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last) : std::multiset<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multiset<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multiset(const multiset & __x) : std::multiset<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        multiset& operator=(const multiset & __x) {
            std::multiset<_Key,_Compare,_Alloc>::operator = (__x) ;
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

// Specialized multiset for other Trick STL types.
template <typename _Key , typename _Compare , typename _Alloc >
class multiset<_Key, _Compare, _Alloc, typename std::enable_if<std::is_base_of<Trick::STLInterface,_Key>::value>::type >
 : public std::multiset<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multiset() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last) : std::multiset<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multiset<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multiset(const multiset & __x) : std::multiset<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        multiset& operator=(const multiset & __x) {
            std::multiset<_Key,_Compare,_Alloc>::operator = (__x) ;
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

// Specialized multiset for strings
template <typename _Key , typename _Compare , typename _Alloc >
class multiset<_Key, _Compare, _Alloc, typename std::enable_if<std::is_same<_Key,std::string>::value>::type >
 : public std::multiset<_Key,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multiset() {} ;

        // iterator range constructor
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last) : std::multiset<_Key,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multiset(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multiset<_Key,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multiset(const multiset & __x) : std::multiset<_Key,_Compare,_Alloc>(__x) {}

        // assignment operator
        multiset& operator=(const multiset & __x) {
            std::multiset<_Key,_Compare,_Alloc>::operator = (__x) ;
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
