/******************************TRICK HEADER*************************************
PURPOSE:                     ( Common definitions for testing the VariableReference class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "trick/MemoryManager.hh"
#include "trick/VariableReference.hh"
#include "trick/UdUnits.hh"

#include <vector>

class NestedObject {
    public:
    int a;
    int b;
};

class TestObject {
    public:
    TestObject() {}

	int length; /*  m xy-position */
    double a;   /*  s time */
    long b;     /*  nautical_mile miles */
    long long c;     /*  in inches */
    unsigned long d;     /*  light_year miles */
    unsigned long long e;     /*  mi miles */
    int error_units; /* asvdsfkj fake units */
    wchar_t * wchar_str;
    std::vector<int> vec;

    NestedObject obj;
};

/*
 Test Fixture.
 */
class VariableReference_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
        Trick::UdUnits * udunits;
		VariableReference_test() { 
            memmgr = new Trick::MemoryManager; 
            udunits = new Trick::UdUnits; 

            udunits->read_default_xml();
        }
		~VariableReference_test() { delete memmgr; }
		void SetUp() {}
		void TearDown() {}
};