/*
    PURPOSE: (Provides a convenient way to instantiate and use a variety of STL containers 
              for C++ types/language bindings with SWIG)
    LIBRARY_DEPENDENCIES: (
      ()
    )
*/


#include <iostream>
#include <string>
#include <cstdint>
#include <cuchar>

#include <list>
#include <queue>
#include <deque>

#include <map>
#include <unordered_map>

#include <set>
#include <unordered_set>

#include <array>
#include <vector>

#include <stack>

#ifdef SWIG
%define DECLARE_SWIG_CONTAINER_TYPES(ALIAS, TYPE)
%template(ALIAS##Array) std::array<TYPE, 3>;
%template(ALIAS##Vector) std::vector<TYPE>;
// Both std_queue.i and std_stack.i coming with swig are wrappers for Ruby.
// We need to come up with our own wrappers for these if needed for Python.
//%template(ALIAS##Queue) std::queue<TYPE>;
//%template(ALIAS##Stack) std::stack<TYPE>;
%template(ALIAS##Deque) std::deque<TYPE>;
%template(ALIAS##Set) std::set<TYPE>;
%template(ALIAS##Map) std::map<TYPE, TYPE>;
#if SWIG_VERSION >= 0x040000
%template(ALIAS##UnorderedMap) std::unordered_map<TYPE, TYPE>;
#endif
%enddef

DECLARE_SWIG_CONTAINER_TYPES(Char, char)
DECLARE_SWIG_CONTAINER_TYPES(SignedChar, signed char)
DECLARE_SWIG_CONTAINER_TYPES(UnsignedChar, unsigned char)
DECLARE_SWIG_CONTAINER_TYPES(Char16T, char16_t)
DECLARE_SWIG_CONTAINER_TYPES(Char32T, char32_t)
DECLARE_SWIG_CONTAINER_TYPES(ShortInt, short int)
DECLARE_SWIG_CONTAINER_TYPES(UnsignedShortInt, unsigned short int)
DECLARE_SWIG_CONTAINER_TYPES(Int, int)
DECLARE_SWIG_CONTAINER_TYPES(UnsignedInt, unsigned int)
DECLARE_SWIG_CONTAINER_TYPES(LongInt, long int)
DECLARE_SWIG_CONTAINER_TYPES(UnsignedLongInt, unsigned long int)
DECLARE_SWIG_CONTAINER_TYPES(LongLongInt, long long int)
DECLARE_SWIG_CONTAINER_TYPES(UnsignedLongLongInt, unsigned long long int)
DECLARE_SWIG_CONTAINER_TYPES(Float, float)
DECLARE_SWIG_CONTAINER_TYPES(Double, double)
DECLARE_SWIG_CONTAINER_TYPES(LongDouble, long double)

#endif


template<typename T>
struct STLContainerTypes
{
    using ArrayType = std::array<T, 3>;
    using VectorType = std::vector<T>;
    using QueueType = std::queue<T>;
    using DequeType = std::deque<T>;
    using SetType = std::set<T>;
    using MapType = std::map<T, T>;
    using UnorderedMapType = std::unordered_map<T, T>;
    using StackType = std::stack<T>;

    ArrayType array;
    VectorType vector;
    QueueType queue;
    DequeType deque;
    SetType set;
    MapType map;
    UnorderedMapType unordered_map;
    StackType stack;

};

