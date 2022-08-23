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
#include "trick/var_binary_parser.hh"


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


TEST_F(VariableServerSession_test, large_message_ascii) {
    // ARRANGE
    Trick::VariableServerSession session(&connection);

    const static int big_arr_size = 4000;
    // Make an array too big to fit in a single message

    int big_arr[big_arr_size];
    for (int i = 0; i < big_arr_size; i++) {
        big_arr[i] = i;
    }

    (void) memmgr->declare_extern_var(&big_arr, "int big_arr[4000]");

    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        session.var_add(var_name.c_str());
    } 

    // ACT
    session.copy_sim_data();
    session.write_data();

    // ASSERT
    ASSERT_TRUE(connection.ascii_messages_written.size() > 1);
    int counter = 0;

    int message_index = 0;
    for (int i = 0; i < connection.ascii_messages_written.size(); i++) {
        std::string message = connection.ascii_messages_written[i];
        std::stringstream ss(message);
        std::string token;

        // First val in first message should be the message type, 0
        if (i == 0) {
            std::getline(ss, token, '\t');
            int message_type = stoi(token);
            EXPECT_EQ(message_type, 0);
        }

        while (std::getline(ss, token, '\t')) {
            if (token == "\n") { break; }

            int num = stoi(token);
            EXPECT_EQ(counter, num);
            counter++;
        }
    }

    EXPECT_EQ(counter, big_arr_size);
}

TEST_F(VariableServerSession_test, large_message_binary) {
    // ARRANGE
    Trick::VariableServerSession session(&connection);
    session.var_binary();

    const static int big_arr_size = 4000;
    // Make an array too big to fit in a single message

    int big_arr[big_arr_size];
    for (int i = 0; i < big_arr_size; i++) {
        big_arr[i] = i;
    }

    (void) memmgr->declare_extern_var(&big_arr, "int big_arr[4000]");

    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        session.var_add(var_name.c_str());
    } 

    // ACT
    session.copy_sim_data();
    session.write_data();

    // ASSERT
    ParsedBinaryMessage full_message;
    for (int i = 0; i < connection.binary_messages_written.size(); i++) {
        ParsedBinaryMessage partial_message;
        partial_message.parse(connection.binary_messages_written[i]);
        full_message.combine(partial_message);
    }

    ASSERT_EQ(full_message.getNumVars(), big_arr_size);
    for (int i = 0; i < big_arr_size; i++) {
        try {
            std::string var_name = "big_arr[" + std::to_string(i) + "]";
            Var variable = full_message.getVariable(var_name);
            
            EXPECT_EQ(variable.getValue<int>(), i);
        } catch (std::exception& ex) {
            FAIL() << ex.what();
        }
    }
}
