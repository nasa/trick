/*************************************************************************
PURPOSE: (Trick TMM Alloc W/ Args)
ICG: (No)
LIBRARY DEPENDENCY:
    (
    ()
    )
**************************************************************************/
#ifndef SWIG
#ifndef __ALLOC_WITH_ARGS_HH__
#define __ALLOC_WITH_ARGS_HH__

#include "trick/trick_type_traits.hh"

/*
In the case that a TrickTypeToString<T> for some type T exist, then this function is valid and will be compiled.
*/
template<typename T, typename ...Args>
typename std::enable_if<Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    void* new_alloc = trick_MM->declare_var(TrickTypeToString<T>::getName().c_str());
    std::cout << "Allocating: " << TrickTypeToString<T>::getName() << std::endl;
    return new (new_alloc) T(std::forward<Args>(args)...);
}

/*
In the case that a TrickTypeToString<T> for some type T does NOT exist, then this function is valid and will be compiled.
*/
template<typename T, typename ...Args>
typename std::enable_if<!Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    static_assert(Trick::always_false<T>::value,
                "You've attempted to call tmm_alloc_args using a type(T) that does not have an implemented specialization for TrickTypeToString.");

    return nullptr;
}

#endif 
#endif