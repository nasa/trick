/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerSession class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "trick/MemoryManager.hh"
#include "trick/UdUnits.hh"

#include "TestConnection.hh"
#include "trick/VariableServerSession.hh"


/*
 Test Fixture.
 */
class VariableServerSession_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
        Trick::UdUnits * udunits;

        TestConnection connection;

		VariableServerSession_test() { 
            memmgr = new Trick::MemoryManager; 
            udunits = new Trick::UdUnits; 

            udunits->read_default_xml();
        }
		~VariableServerSession_test() { delete memmgr; }
		void SetUp() {}
		void TearDown() {}
};

TEST_F(VariableServerSession_test, exit) {
    Trick::VariableServerSession session(&connection);

    ASSERT_EQ(session.exit_cmd, false);

    session.var_exit();

    ASSERT_EQ(session.exit_cmd, true);
}

