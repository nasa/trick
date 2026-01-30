/*
    PURPOSE: (Defines the STLIPTest class, which serves as a testbed for various STL
              container types instantiated with C++ types such as int, double, char, etc.)
    LIBRARY_DEPENDENCIES: (
      ()
    )
*/

#ifndef STL_IP_TEST_HH
#define STL_IP_TEST_HH

#include <cstdint>
#include <cuchar>
#include <iostream>

#include <deque>
#include <list>
#include <queue>

#include <map>
#include <unordered_map>

#include <set>
#include <unordered_set>

#include <array>
#include <vector>

#include <memory>

#include <cstddef>
#include <string>

#include "test_classes/include/container_types.hh"

struct TestStructure
{
    int a;
    double b;
};

class STLIPTest
{
    friend class InputProcessor;

public:
    STLIPTest() = default;
    ~STLIPTest() = default;

    // STLs
    // Char Types

    // inline char16_t to_char16_t(char c) {return static_cast<char16_t>(c);}

    // Python doesn't have a char16_t type, so we use int for compatibility
    // Add a character to the vector (accepts int for Python compatibility)
    void addCharToVector(std::vector<char16_t> & vec, int value)
    {
        vec.push_back(static_cast<char16_t>(value));
    }

    // Get a character from the vector (returns as int for Python compatibility)
    int getCharFromVector(std::vector<char16_t> & vec, size_t index)
    {
        if(index >= vec.size())
        {
            // Handle out of bounds access
            throw std::out_of_range("Index out of bounds");
        }
        // Convert char16_t to int when returning to Python
        return static_cast<int>(vec[index]);
    }

    // Python doesn't have a char32_t type, so we use int for compatibility
    // Add a character to the vector (accepts int for Python compatibility)
    void addCharToVector(std::vector<char32_t> & vec, int value)
    {
        vec.push_back(static_cast<char32_t>(value));
    }

    // Get a character from the vector (returns as int for Python compatibility)
    int getCharFromVector(std::vector<char32_t> & vec, size_t index)
    {
        if(index >= vec.size())
        {
            // Handle out of bounds access
            throw std::out_of_range("Index out of bounds");
        }
        // Convert char16_t to int when returning to Python
        return static_cast<int>(vec[index]);
    }

    STLContainerTypes<char> char_types;
    STLContainerTypes<signed char> signed_char_types;
    STLContainerTypes<unsigned char> unsigned_char_types;
    STLContainerTypes<char16_t> char16_t_types;
    STLContainerTypes<char32_t> char32_t_types;
    STLContainerTypes<short int> short_int_types;
    STLContainerTypes<unsigned short int> unsigned_short_int_types;
    STLContainerTypes<int> int_types;
    STLContainerTypes<unsigned int> unsigned_int_types;
    STLContainerTypes<long int> long_int_types;
    STLContainerTypes<unsigned long int> unsigned_long_int_types;
    STLContainerTypes<long long int> long_long_int_types;
    STLContainerTypes<unsigned long long int> unsigned_long_long_int_types;
    STLContainerTypes<float> float_types;
    STLContainerTypes<double> double_types;
    STLContainerTypes<long double> long_double_types;
};

#endif
