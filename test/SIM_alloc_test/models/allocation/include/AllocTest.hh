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

#include <iostream>
#include "sim_services/include/mm_macros.hh"

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

#endif /* _BALL_HH_ */

