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

/**

 @tparam T Type to be allocated.
 @tparam Args parameter pack for forward arguments to placement new.
 @details Wrapper around tricks MemoryManager void* declare_var( const char* declaration);)  Users can pass constructor 
 arguments to this function and it will construct type T in the memory return by declare_var
 @param Args  Parameter pack of arguments pased to the constructor
 @return pointer to type T that was just allocated.
*/

template<typename T, typename ...Args>
typename std::enable_if<Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    void* new_alloc = trick_MM->declare_var(TrickTypeToString<T>::getName().c_str());
    return new (new_alloc) T(std::forward<Args>(args)...);
}

/**

 @tparam T Type to be allocated.
 @tparam Args parameter pack for forward arguments to placement new.
 @details Convenience funcion.  No memory allocation, let's user pass in existing valid memory address and construct
 the type T in that memory via placement new.
 @param Args  Parameter pack of arguments pased to the constructor 
 @param address Memory address the call to T() will be constructed in.
 @return pointer to type T that was just allocated.
*/
template<typename T, typename ...Args>
typename std::enable_if<Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(T* address, Args&&... args)
{
    return new (address) T(std::forward<Args>(args)...);
}

/**
 * 
 @tparam T Type to be allocated.
 @tparam Args parameter pack for forward arguments to placement new.
 @details Version of tmm_alloc_args that will be compiled if TrickTypeToString has no 
 member function 'getName()'.  Static assert will end compilation and print a useful
 error message.
  @return Technically a nullptr, but this will never compile.
*/
template<typename T, typename ...Args>
typename std::enable_if<!Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    //Compilation will always fail here.
    static_assert(Trick::always_false<T>::value,
                "You've attempted to call tmm_alloc_args using a type(T) that does not have an implemented specialization for TrickTypeToString.");

    return nullptr;
}

#endif 
#endif