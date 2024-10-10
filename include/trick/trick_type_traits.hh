/*************************************************************************
PURPOSE: (Trick Type Traits)
ICG: (No)
LIBRARY DEPENDENCY:
    (
    ()
    )
**************************************************************************/

#ifndef SWIG
#ifndef __TMM_ALLOC_ARGS_HH__
#define __TMM_ALLOC_ARGS_HH__

#include <utility>
#include <type_traits>
#include "trick_types.hh"

#include "trick/MemoryManager.hh"

#if defined (__GNUC__)
#define TRICK_HAVE_ABI
#elif
#if __has_include(<cxxabi.h>)
#define TRICK_HAVE_ABI
#endif
#endif

namespace Trick {


// Helper struct to always yield false
template<typename T>
struct always_false : std::false_type {};

//C++11 work around to get a void_t type
template<typename... Ts>
struct make_void {
    typedef void type;
};

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

//Helper struct so we can check if a type has a function "has_getname" defaults to false
template<typename T, typename = void_t<>>
struct has_getname : std::false_type {};

template<typename T>
struct has_getname<T, void_t<decltype(std::declval<TrickTypeToString<T>>().getName())>> : std::true_type {};


}


#endif //__TMM_ALLOC_ARGS_HH__

#endif