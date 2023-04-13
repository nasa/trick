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

#include "trick/RealtimeSync.hh"
#include "trick/GetTimeOfDayClock.hh"
#include "trick/ITimer.hh"

#include "trick/VariableServerSession.hh"
#include "trick/var_binary_parser.hh"

#include "MockClientConnection.hh"
#include "MockVariableServerSession.hh"



using ::testing::AtLeast;
using ::testing::_;
using ::testing::Truly;
using ::testing::Args;
using ::testing::Return;
using ::testing::Invoke;


/*
 Test Fixture.
 */
class VariableServerSession_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
        Trick::UdUnits * udunits;
        Trick::RealtimeSync * realtime_sync;

        Trick::GetTimeOfDayClock clock;
        Trick::ITimer timer;


        MockClientConnection connection;

		VariableServerSession_test() { 
            memmgr = new Trick::MemoryManager; 
            udunits = new Trick::UdUnits; 
            realtime_sync = new Trick::RealtimeSync(&clock, &timer);

            udunits->read_default_xml();
        }
		~VariableServerSession_test() { 
            delete memmgr; 
            delete realtime_sync;
        }

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

    // Create references for all of them
    std::vector <Trick::VariableReference *> vars;
    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        Trick::VariableReference * var = new Trick::VariableReference(var_name);
        var->stageValue();
        vars.push_back(var);
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
    session.write_data(vars, (VS_MESSAGE_TYPE) 0);

    // ASSERT
    // Make sure that the entire message was written out
    ASSERT_EQ(val_counter, big_arr_size);
}

TEST_F(VariableServerSession_test, DISABLED_large_message_binary) {
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

    // Create references for all of them
    std::vector <Trick::VariableReference *> vars;
    for (int i = 0; i < big_arr_size; i++) {
        std::string var_name = "big_arr[" + std::to_string(i) + "]";
        Trick::VariableReference * var = new Trick::VariableReference(var_name);
        var->stageValue();
        vars.push_back(var);
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
    session.write_data(vars, (VS_MESSAGE_TYPE) 0);

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


void setup_partial_session_mock(MockVariableServerSession& session) {
    EXPECT_CALL(session, copy_and_write_async())
        .WillOnce(Invoke(&session, &MockVariableServerSession::copy_and_write_async_concrete));
}

void set_session_modes(MockVariableServerSession& session, VS_COPY_MODE copy_mode, VS_WRITE_MODE write_mode, bool pause) {
    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(copy_mode));
    EXPECT_CALL(session, get_write_mode())
        .WillRepeatedly(Return(write_mode));
    EXPECT_CALL(session, get_pause())
        .WillOnce(Return(pause));   
}


TEST_F(VariableServerSession_test, copy_async_disabled) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set disabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));

    // Nothing else should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    session.copy_and_write_async();
}


TEST_F(VariableServerSession_test, copy_async_copy_and_write) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write when copied, not paused
    set_session_modes(session, VS_COPY_ASYNC, VS_WRITE_ASYNC, false);

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_GE(result, 0);
}

TEST_F(VariableServerSession_test, copy_async_copy_write_when_copied) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write when copied, not paused
    set_session_modes(session, VS_COPY_ASYNC, VS_WRITE_WHEN_COPIED, false);

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_GE(result, 0);
}

TEST_F(VariableServerSession_test, copy_async_no_copy_or_write) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write when copied, not paused
    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;
    

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_GE(result, 0);
}

TEST_F(VariableServerSession_test, copy_async_non_realtime) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write async, paused
    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = false;

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_GE(result, 0);
}

TEST_F(VariableServerSession_test, copy_async_paused) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write async, paused
    set_session_modes(session, VS_COPY_ASYNC, VS_WRITE_ASYNC, true);

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_GE(result, 0);
}


TEST_F(VariableServerSession_test, copy_async_write_fails) {
    // ARRANGE
    MockVariableServerSession session;
    setup_partial_session_mock(session);

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    // Copy async, write async, paused
    set_session_modes(session, VS_COPY_ASYNC, VS_WRITE_ASYNC, false);

    // Copy and write should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .WillOnce(Return(-1));

    // Exit should be called
    EXPECT_CALL(session, set_exit_cmd())
        .Times(1);

    // ACT
    int result = session.copy_and_write_async();

    // ASSERT
    ASSERT_EQ(result, -1);
}


TEST_F(VariableServerSession_test, copy_and_write_top_disabled) {
   // ARRANGE
    MockVariableServerSession session;

    // set disabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));

    // Nothing else should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    session.copy_and_write_top(100);
}

TEST_F(VariableServerSession_test, copy_and_write_top_copy_top_write_async) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_ASYNC, false);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_top(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_top_copy_top_write_when_copied) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_top(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_top_copy_top_dont_write_if_non_realtime) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = false;

    EXPECT_CALL(session, get_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_top(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_top_copy_top_write_paused) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));
 
    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, true);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_top(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_top_wrong_offset) {
    // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));
    EXPECT_CALL(session, get_write_mode())
        .WillRepeatedly(Return(VS_WRITE_WHEN_COPIED));

    realtime_sync->active = true;

    EXPECT_CALL(session, get_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_top(101);

    // ASSERT
    EXPECT_EQ(result, 0);
}



TEST_F(VariableServerSession_test, copy_and_write_freeze_disabled) {
   // ARRANGE
    MockVariableServerSession session;

    // set disabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));

    // Nothing else should be called
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    session.copy_and_write_freeze(100);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_copy_top_write_async) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_ASYNC, false);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_freeze_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_freeze_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_copy_top_write_when_copied) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_freeze_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_freeze_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_freeze(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_copy_top_dont_write_if_non_realtime) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = false;

    EXPECT_CALL(session, get_freeze_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_freeze_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_copy_top_write_paused) {
   // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_TOP_OF_FRAME, VS_WRITE_WHEN_COPIED, true);
    realtime_sync->active = true;

    EXPECT_CALL(session, get_freeze_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_freeze_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_freeze_wrong_offset) {
    // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));
    EXPECT_CALL(session, get_write_mode())
        .WillRepeatedly(Return(VS_WRITE_WHEN_COPIED));

    realtime_sync->active = true;

    EXPECT_CALL(session, get_freeze_frame_multiple())
        .WillRepeatedly(Return(10));
    EXPECT_CALL(session, get_freeze_frame_offset())
        .WillRepeatedly(Return(0));

    // Should only copy
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze(101);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_disabled) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_wrong_mode) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));
    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_wrong_tics) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));
    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(10));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_scheduled_copy_and_write) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_copy_scheduled_write_async) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_paused) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, true);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_scheduled_non_realtime) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, false);
    realtime_sync->active = false;
    
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_scheduled_write_fails) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_next_tics())
        .WillRepeatedly(Return(100));

    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .WillOnce(Return(-1));

    EXPECT_CALL(session, set_exit_cmd())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, -1);
}


TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_disabled) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_wrong_mode) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));
    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_wrong_tics) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(false));
    EXPECT_CALL(session, get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_TOP_OF_FRAME));
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(10));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(0);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_copy_and_write) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_copy_scheduled_write_async) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_paused) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, true);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}

TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_non_realtime) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_ASYNC, false);
    realtime_sync->active = false;
    
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(100));

    // Shouldn't do anything
    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .Times(0);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, 0);
}


TEST_F(VariableServerSession_test, copy_and_write_freeze_scheduled_write_fails) {
     // ARRANGE
    MockVariableServerSession session;

    // set enabled
    EXPECT_CALL(session, get_enabled())
        .WillOnce(Return(true));

    set_session_modes(session, VS_COPY_SCHEDULED, VS_WRITE_WHEN_COPIED, false);
    realtime_sync->active = true;
    
    EXPECT_CALL(session, get_freeze_next_tics())
        .WillRepeatedly(Return(100));

    EXPECT_CALL(session, copy_sim_data())
        .Times(1);

    EXPECT_CALL(session, write_data())
        .WillOnce(Return(-1));

    EXPECT_CALL(session, set_exit_cmd())
        .Times(1);
    
    // ACT
    int result = session.copy_and_write_freeze_scheduled(100);

    // ASSERT
    EXPECT_EQ(result, -1);
}