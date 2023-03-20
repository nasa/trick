/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerThread class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "trick/MemoryManager.hh"
#include "trick/UdUnits.hh"
#include "trick/VariableServer.hh"
#include "trick/VariableServerThread.hh"
#include "trick/var_binary_parser.hh"

#include "MockClientConnection.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

/*
 Test Fixture.
 */
class VariableServerThread_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager * memmgr;
        Trick::UdUnits * udunits;
        Trick::VariableServer * varserver;

        MockClientConnection connection;

		VariableServerThread_test() { 
            memmgr = new Trick::MemoryManager; 
            udunits = new Trick::UdUnits; 
            varserver = new Trick::VariableServer;

            Trick::VariableServerThread::set_vs_ptr(varserver);

            udunits->read_default_xml();
        }

		~VariableServerThread_test() { 
            delete memmgr;
            delete varserver; 
        }

		void SetUp() {}
		void TearDown() {}
};


TEST_F(VariableServerThread_test, connection_failure) {
    // ARRANGE

    // Starting the connection fails, disconnect should be called
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(1));
    EXPECT_CALL(connection, disconnect())
        .Times(1);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    vst->join_thread();

    // ASSERT
    EXPECT_EQ(status, Trick::ConnectionStatus::CONNECTION_FAIL);

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerThread_test, connection_closed) {
    // ARRANGE

    // Starting the connection fails, disconnect should be called
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(0));
    
    // Read once, call 
    EXPECT_CALL(connection, read(_,_))
        .Times(1)
        .WillOnce(Return(-1));;
        
    EXPECT_CALL(connection, disconnect())
        .Times(1);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    vst->join_thread();

    // ASSERT

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerThread_test, connection_exits) {
    // ARRANGE

    // Starting the connection fails, disconnect should be called
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(0));
    
    // Read as many times as we need
    EXPECT_CALL(connection, read(_,_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(0));
        
    EXPECT_CALL(connection, disconnect())
        .Times(1);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread ;
    vst->set_connection(&connection);
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * session = varserver->get_session(id);
    ASSERT_TRUE(session != NULL);

    // ACT
    session->set_exit_cmd();

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerThread_test, connection_shuts_down_on_cancel) {
    // ARRANGE

    // Starting the connection fails, disconnect should be called
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(0));
    
    // Read as many times as we need
    EXPECT_CALL(connection, read(_,_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(0));
        
    EXPECT_CALL(connection, disconnect())
        .Times(1);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread ;
    vst->set_connection(&connection);
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * session = varserver->get_session(id);
    ASSERT_TRUE(session != NULL);

    // ACT
    vst->cancel_thread();

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}