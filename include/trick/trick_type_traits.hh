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

/*
In the case that a TrickTypeToString<T> for some type T exist, then this function is valid and will be compiled.
*/
template<typename T, typename ...Args>
typename std::enable_if<has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    void* new_alloc = trick_MM->declare_var(TrickTypeToString<T>::getName().c_str());
    return new (new_alloc) T(std::forward<Args>(args)...);
}

/*
In the case that a TrickTypeToString<T> for some type T does NOT exist, then this function is valid and will be compiled.
*/
template<typename T, typename ...Args>
typename std::enable_if<!has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    static_assert(always_false<T>::value,
                "You've attempted to call tmm_alloc_args using a type(T) that does not have an implemented specialization for TrickTypeToString.");

    return nullptr;
}


}

#endif //__TMM_ALLOC_ARGS_HH__

#endif