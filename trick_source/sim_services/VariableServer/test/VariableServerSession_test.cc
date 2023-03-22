/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerSession class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "trick/MemoryManager.hh"
#include "trick/UdUnits.hh"

#include "trick/VariableServerSession.hh"
#include "trick/var_binary_parser.hh"

#include "MockClientConnection.hh"


using ::testing::AtLeast;
using ::testing::_;
using ::testing::Truly;
using ::testing::Args;

/*
 Test Fixture.
 */
class VariableServerSession_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
        Trick::UdUnits * udunits;

        MockClientConnection connection;

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

    Trick::VariableServerSession session;

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
    Trick::VariableServerSession session;

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
    Trick::VariableServerSession session;
    session.set_connection(&connection);

    const static int big_arr_size = 4000;
    // Make an array too big to fit in a single message

    int big_arr[big_arr_size];
    for (int i = 0; i < big_arr_size; i++) {
        big_arr[i] = i;
    }

    // Set it up with the memory manager
    (void) memmgr->declare_extern_var(&big_arr, "int big_arr[4000]");

    // Add it all to the session
    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        session.var_add(var_name.c_str());
    } 

    // Make a matcher for the Mock
    // This will check that the messages that are passed into connection.write() are correct
    int val_counter = 0;
    int message_counter = 0;
    int final_val = 0;
    auto constructedCorrectly = [&] (std::string s) -> bool {
        std::stringstream ss(s);
        std::string token;

        if (message_counter == 0) {
            // First val in first message should be the message type, 0
            std::getline(ss, token, '\t');
            int message_type = stoi(token);
            if (message_type != 0) return false;
        } else {
            // First val in all other message should be (last val in previous message)+1
            std::getline(ss, token, '\t');
            int first_num = stoi(token);
            if (first_num != final_val+1) return false;
            val_counter++;
        }

        int num = 0;

        // The rest of the message should just be sequential
        while (std::getline(ss, token, '\t')) {
            if (token == "\n") { break; }
            
            num = stoi(token);
            if (val_counter != num) return false;
            val_counter++;
        }

        // Save the final value so that we can verify the next message starts correctly
        final_val = num;
        message_counter++;

        return true;
    };

    // Set up the mock connection - it should make 3 calls to write
    // The constructedCorrectly matcher will ensure that the values passed in are what we want them to be
    EXPECT_CALL(connection, write(Truly(constructedCorrectly)))
        .Times(3);

    // ACT
    session.copy_sim_data();
    session.write_data();

    // ASSERT
    // Make sure that the entire message was written out
    ASSERT_EQ(val_counter, big_arr_size);
}

TEST_F(VariableServerSession_test, large_message_binary) {
    // ARRANGE
    Trick::VariableServerSession session;
    session.set_connection(&connection);
    session.var_binary();

    const static int big_arr_size = 4000;
    // Make an array too big to fit in a single message

    int big_arr[big_arr_size];
    for (int i = 0; i < big_arr_size; i++) {
        big_arr[i] = i;
    }

    // Set it up with the memory manager
    (void) memmgr->declare_extern_var(&big_arr, "int big_arr[4000]");

    // Add it all into the session
    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        session.var_add(var_name.c_str());
    } 

    // Create a matcher that parses and collects the arguments into a ParsedBinaryMessage
    ParsedBinaryMessage full_message;
    auto binaryConstructedCorrectly = [&] (std::tuple<char *, int> msg_tuple) -> bool {
        char * message;
        int size;
        std::tie(message, size) = msg_tuple;
        ParsedBinaryMessage partial_message;

        // Put the message into a vector the parser
        std::vector<unsigned char> bytes;
        for (int i = 0; i < size; i++) {
            bytes.push_back(message[i]);
        }

        // Parse and add it to the full message
        try {
            partial_message.parse(bytes);
            full_message.combine(partial_message);
        } catch (const MalformedMessageException& ex) {
            std::cout << "Parser failed with message: " << ex.what();
            return false;
        }

        return true;
    };

    // Set up the mock connection - it should make a bunch of calls to write
    // The constructedCorrectly matcher will ensure that the values passed in are what we want them to be
    EXPECT_CALL(connection, write(_, _)).With(Args<0,1>(Truly(binaryConstructedCorrectly))).Times(AtLeast(3));

    // ACT
    session.copy_sim_data();
    session.write_data();

    // ASSERT
    
    // Make sure we got all of the values
    ASSERT_EQ(full_message.getNumVars(), big_arr_size);

    // Make sure all of the values are correct and have names
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
