/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerSessionThread class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>

#include "trick/VariableServer.hh"
#include "trick/ExecutiveException.hh"
#include "trick/message_type.h"

#include "trick/VariableServerSessionThread.hh"

#include "trick/Mock/MockMessagePublisher.hh"
#include "trick/Mock/MockVariableServerSession.hh"
#include "trick/Mock/MockClientConnection.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Throw;
using ::testing::Const;
using ::testing::NiceMock;


// Set up default mock behavior

void setup_default_connection_mocks (MockClientConnection * connection) {
    // Starting the connection succeeds
    ON_CALL(*connection, start())
        .WillByDefault(Return(0));
    
    // We should always get a disconnect call
    ON_CALL(*connection, disconnect())
        .WillByDefault(Return(0));
}

void setup_default_session_mocks (MockVariableServerSession * session, bool command_exit = true) {
    ON_CALL(*session, handle_message())
        .WillByDefault(Return(0));

    ON_CALL(*session, copy_and_write_async())
        .WillByDefault(Return(0));

    ON_CALL(*session, get_pause())
        .WillByDefault(Return(false));
        
    ON_CALL(Const(*session), get_update_rate())
        .WillByDefault(Return(0.001));

    ON_CALL(*session, get_exit_cmd())
        .WillByDefault(Return(false));
}

/*
 Test Fixture.
 */
class VariableServerSessionThread_test : public ::testing::Test {
	protected:
        Trick::VariableServer * varserver;

        MockClientConnection connection;
        NiceMock<MockVariableServerSession> * session;

        MockMessagePublisher message_publisher;

		VariableServerSessionThread_test() { 
            // Set up dependencies that haven't been broken
            varserver = new Trick::VariableServer;
            Trick::VariableServerSessionThread::set_vs_ptr(varserver);

            // Set up mocks
            session = new  NiceMock<MockVariableServerSession>;
            setup_default_connection_mocks(&connection);
            setup_default_session_mocks(session);
        }

		~VariableServerSessionThread_test() { 
            delete varserver; 
        }

		void SetUp() {}
		void TearDown() {}
};


// Helper functions for common cases of Mock expectations

void setup_normal_connection_expectations (MockClientConnection * connection) {
    // Starting the connection succeeds
    EXPECT_CALL(*connection, start())
        .Times(1)
        .WillOnce(Return(0));
    
    // We should always get a disconnect call
    EXPECT_CALL(*connection, disconnect())
        .Times(1);
}

void set_session_exit_after_some_loops(MockVariableServerSession * session) {
    EXPECT_CALL(*session, get_exit_cmd())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
}

TEST_F(VariableServerSessionThread_test, connection_failure) {
    // ARRANGE

    // Starting the connection fails
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(1));
    
    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    vst->join_thread();

    // ASSERT
    EXPECT_EQ(status, Trick::ConnectionStatus::CONNECTION_FAIL);

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);

    delete session;
}


TEST_F(VariableServerSessionThread_test, DISABLED_exit_if_handle_message_fails) {

    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Handle a message, but it fails
    EXPECT_CALL(*session, handle_message())
        .Times(1)
        .WillOnce(Return(-1));
    
        
    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    vst->join_thread();

    // ASSERT

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerSessionThread_test, DISABLED_exit_if_write_fails) {

    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Write out data
    EXPECT_CALL(*session, copy_and_write_async())
        .WillOnce(Return(-1));

        
    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    vst->join_thread();

    // ASSERT

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerSessionThread_test, exit_commanded) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Runs for a few loops, then exits

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerSessionThread_test, thread_cancelled) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // ACT
    vst->cancel_thread();

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerSessionThread_test, turn_session_log_on) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    varserver->set_var_server_log_on();

    // We expect a the session's log to be turned on
    EXPECT_CALL(*session, set_log(true))
        .Times(1);

    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerSessionThread_test, throw_trick_executive_exception) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);

    EXPECT_CALL(*session, get_exit_cmd())
        .WillRepeatedly(Return(false));

    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    EXPECT_CALL(*session, handle_message())
        .WillOnce(Throw(Trick::ExecutiveException(-1, __FILE__, __LINE__, "Trick::ExecutiveException Error message for testing")));

    EXPECT_CALL(message_publisher, publish(MSG_ERROR, _));

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerSessionThread_test, throw_exception) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);

    EXPECT_CALL(*session, get_exit_cmd())
        .WillRepeatedly(Return(false));

    // Set up VariableServerSessionThread
    Trick::VariableServerSessionThread * vst = new Trick::VariableServerSessionThread(session) ;
    vst->set_connection(&connection);

    EXPECT_CALL(*session, handle_message())
        .WillOnce(Throw(std::logic_error("Error message for testing")));
    EXPECT_CALL(message_publisher, publish(MSG_ERROR, _));


    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerSessionThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}
