/*
    PURPOSE: (Illustrate how to checkpoint STLs)
    LIBRARY_DEPENDENCIES: (
      (STLIPTest.o)
    )
*/

#ifndef STL_IP_TEST_HH
#define STL_IP_TEST_HH

#include <iostream>
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

#include <memory>

#include <string>
#include <cstddef>

#include "utils.hh"

// Define the macro that does the declaration.
#define DECLARE_ARRAY(Type, Suffix) std::array<Type, 3> array_##Suffix##_3;
#define DECLARE_VECTOR(Type, Suffix) std::vector<Type> vector_##Suffix##_n;
#define DECLARE_QUEUE(Type, Suffix) std::queue<Type> queue_##Suffix##_n;
#define DECLARE_DEQUEUE(Type, Suffix) std::deque<Type> deque_##Suffix##_n;
#define DECLARE_SET(Type, Suffix) std::set<Type> set_##Suffix##_n;


class STLIPTest 
{
    friend class InputProcessor;

    public:
    STLIPTest() = default;
    ~STLIPTest() = default;

    //STLs
    //Char Types
    FOREACH_TYPE(DECLARE_ARRAY);
    FOREACH_TYPE(DECLARE_VECTOR);
    FOREACH_TYPE(DECLARE_QUEUE);
    FOREACH_TYPE(DECLARE_DEQUEUE);
    FOREACH_TYPE(DECLARE_SET);
};  

#endif