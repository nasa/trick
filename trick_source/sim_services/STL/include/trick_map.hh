
#ifndef TRICK_MAP_HH
#define TRICK_MAP_HH

#include <string>
#include <map>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_map_stl.hh"
#include "sim_services/STL/include/STLUtilities.hh"

namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
          typename _Alloc = std::allocator<_Tp> , class dummy = void >
class map {} ;

// Template used when both key and data are not strings or other STLs
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map normal key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map normal key, STL data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map string key, normal data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map string key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map string key, STL data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map STL key, normal data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map STL key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized map STL key, STL data.  (Really? Who in their right mind would do this?)
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class map<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::map<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        map() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         map(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::map<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        map(const map & __x) : std::map<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        map& operator=(const map & __x) {
            std::map<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

/* MULTIMAP */

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
          typename _Alloc = std::allocator<_Tp> , class dummy = void >
class multimap {} ;

// Template used when both key and data are not strings or other STLs
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap normal key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap normal key, STL data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< !std::is_same<_Key,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_nkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_nkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap string key, normal data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap string key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap string key, STL data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_same<_Key,std::string>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_skey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_skey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap STL key, normal data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          !std::is_same<_Tp,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap STL key, string data.
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_same<_Tp,std::string>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized multimap STL key, STL data.  (Really? Who in their right mind would do this?)
template <typename _Key, typename _Tp , typename _Compare, typename _Alloc >
class multimap<_Key, _Tp, _Compare, _Alloc,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_Key>::value &&
                          std::is_base_of<Trick::STLInterface,_Tp>::value
                        >::type > : public std::multimap<_Key,_Tp,_Compare,_Alloc> , public STLInterface {
    public:
        // default constructor
        multimap() {} ;

        // iterator range constructor
        template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last) {}

        // iterator range constructor with compare function
        template<typename _InputIterator>
         multimap(_InputIterator __first, _InputIterator __last,
          const _Compare& __comp,
          const _Alloc& __a = _Alloc()) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__first, __last,__comp,__a) {}

        // copy constructor
        multimap(const multimap & __x) : std::multimap<_Key,_Tp,_Compare,_Alloc>(__x) {}

        // assignment operator
        multimap& operator=(const multimap & __x) {
            std::multimap<_Key,_Tp,_Compare,_Alloc>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_map_stlkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_map_stlkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

} ;

#endif
