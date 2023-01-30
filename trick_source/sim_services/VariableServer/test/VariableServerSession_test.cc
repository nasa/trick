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

TEST_F(VariableServerSession_test, toString) {

    int a = 5;
    double b = 6;
    std::string c = "Hello";
    (void) memmgr->declare_extern_var(&a, "int a");
    (void) memmgr->declare_extern_var(&b, "double b");
    (void) memmgr->declare_extern_var(&c, "std::string c");

    Trick::VariableServerSession session(&connection);

    session.var_add("a");
    session.var_add("b");
    session.var_add("c");
    session.var_binary();

    std::stringstream ss;
    ss << session;

    std::string expected = "    \"format\":\"BINARY\",\n    \"update_rate\":0.1,\n    \"variables\":[\n      \"a\",\n      \"b\",\n      \"c\"\n    ]\n";
    EXPECT_EQ(ss.str(), expected);

    session.var_ascii();
    expected = "    \"format\":\"ASCII\",\n    \"update_rate\":0.1,\n    \"variables\":[\n      \"a\",\n      \"b\",\n      \"c\"\n    ]\n";
    ss.str("");
    ss << session;
    EXPECT_EQ(ss.str(), expected);
}

TEST_F(VariableServerSession_test, var_sync) {
    // ARRANGE
    Trick::VariableServerSession session(&connection);


    // ACT
    session.var_sync(0);
    // ASSERT
    ASSERT_EQ(session.get_copy_mode(), VS_COPY_ASYNC);
    ASSERT_EQ(session.get_write_mode(), VS_WRITE_ASYNC);


    // ACT
    session.var_sync(1);
    // ASSERT
    ASSERT_EQ(session.get_copy_mode(), VS_COPY_SCHEDULED);
    ASSERT_EQ(session.get_write_mode(), VS_WRITE_ASYNC);

    // ACT
    session.var_sync(2);
    // ASSERT
    ASSERT_EQ(session.get_copy_mode(), VS_COPY_SCHEDULED);
    ASSERT_EQ(session.get_write_mode(), VS_WRITE_WHEN_COPIED);
}


TEST_F(VariableServerSession_test, exit) {
    Trick::VariableServerSession session(&connection);

    ASSERT_EQ(session.exit_cmd, false);

    session.var_exit();

    ASSERT_EQ(session.exit_cmd, true);
}

