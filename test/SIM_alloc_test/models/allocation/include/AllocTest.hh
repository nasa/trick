/**
@file

@verbatim
PURPOSE:
    (Test all the ways to allocate and free/delete a class.)
@endverbatim
*******************************************************************************/

#ifndef ALLOCTEST_HH
#define ALLOCTEST_HH

// System include files.

#include "trick/mm_macros.hh"

#include <iostream>

class AllocTest {
    public:
        int ii ;
        AllocTest() : ii(0) { std::cout << "AllocTest construct" << std::endl ; } ;
        ~AllocTest() { std::cout << "AllocTest destruct" << std::endl ; } ;
} ;

class AllocTestWithMMInterface {
    TRICK_MM_INTERFACE(AllocTestWithMMInterface, AllocTestWithMMInterface)
    public:
        int ii ;
        AllocTestWithMMInterface() : ii(0) { std::cout << "AllocTestWithMMInterface construct" << std::endl ; } ;
        ~AllocTestWithMMInterface() { std::cout << "AllocTestWithMMInterface destruct" << std::endl ; } ;
} ;

class BaseClassAllocTest
{
    public:
        void noop_base() { std::cout << "BaseClassAllocTest noop_base()" << std::endl; };
};

class DerivedClassAllocTest : public BaseClassAllocTest
{
        int marker { 424242 };

    public:
        void noop_derived() { std::cout << "DerivedClassAllocTest noop_derived() marker=" << marker << std::endl; };
};

#endif /* _BALL_HH_ */

