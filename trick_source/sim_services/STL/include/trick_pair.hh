
#ifndef TRICK_PAIR_HH
#define TRICK_PAIR_HH

#include <string>
#include <utility>
#ifndef SWIG
#include <type_traits>
#endif
#include "sim_services/STL/include/checkpoint_trick_pair_stl.hh"
#include "sim_services/STL/include/STLUtilities.hh"


namespace Trick {

// Empty template.  This version is not used, it allows us to specialize below.
template <typename _T1, typename _T2, class dummy = void >
class pair {} ;

// Template used when both key and data are not strings or other STLs
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< !std::is_same<_T1,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T1>::value &&
                          !std::is_same<_T2,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {}

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_nkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_nkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair normal key, string data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< !std::is_same<_T1,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T1>::value &&
                          std::is_same<_T2,std::string>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_nkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_nkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair normal key, STL data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< !std::is_same<_T1,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T1>::value &&
                          std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_nkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_nkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair string key, normal data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_same<_T1,std::string>::value &&
                          !std::is_same<_T2,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_skey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_skey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair string key, string data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_same<_T1,std::string>::value &&
                          std::is_same<_T2,std::string>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_skey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_skey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair string key, STL data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_same<_T1,std::string>::value &&
                          std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_skey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_skey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair STL key, normal data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_T1>::value &&
                          !std::is_same<_T2,std::string>::value &&
                          !std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_stlkey_ndata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_stlkey_ndata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair STL key, string data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_T1>::value &&
                          std::is_same<_T2,std::string>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_stlkey_sdata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_stlkey_sdata( *this , obj_name , stl_id ) ;
        }
} ;

// Specialized pair STL key, STL data.
template <typename _T1, typename _T2 >
class pair<_T1, _T2,
 typename std::enable_if< std::is_base_of<Trick::STLInterface,_T1>::value &&
                          std::is_base_of<Trick::STLInterface,_T2>::value
                        >::type > : public std::pair<_T1,_T2> , public STLInterface {
    public:
        // default constructor
        pair() {} ;

        // incoming data constructor
        pair(const _T1& __a, const _T2& __b) : std::pair<_T1,_T2>(__a,__b) {}

        // templated copy constructor
        template<class _U1, class _U2>
         pair(const pair<_U1, _U2>& __p) : std::pair<_T1,_T2>(__p) {}

        // assignment operator
        pair& operator=(const pair & __x) {
            std::pair<_T1,_T2>::operator = (__x) ;
            return *this ;
        }

        // inherited STLInterface::checkpoint
        virtual void checkpoint( std::string obj_name ) {
            checkpoint_trick_pair_stlkey_stldata( *this , obj_name , stl_id ) ;
        }

        // inherited STLInterface::checkpoint
        virtual void post_checkpoint( std::string obj_name ) {
            delete_trick_map_stl( obj_name , stl_id ) ;
        }

        // inherited STLInterface::restart
        virtual void restart( std::string obj_name ) {
            restore_trick_pair_stlkey_stldata( *this , obj_name , stl_id ) ;
        }
} ;

template<class _T1, class _T2>
inline pair<_T1, _T2> make_pair(_T1 __x, _T2 __y) { return pair<_T1, _T2>(__x, __y); }

} ;

#endif
