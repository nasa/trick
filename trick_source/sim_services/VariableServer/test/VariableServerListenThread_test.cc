/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerListenThread class )
*******************************************************************************/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "trick/CommandLineArguments.hh"
#include "trick/VariableServer.hh"

#include "trick/message_type.h"

#include "trick/Mock/MockExecutive.hh"
#include "trick/Mock/MockMessagePublisher.hh"

#include "trick/Mock/MockMulticastGroup.hh"
#include "trick/Mock/MockTCPClientListener.hh"
#include "trick/Mock/MockTCPConnection.hh"

#include "trick/VariableServerListenThread.hh"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

/*
 Test Fixture.
 */
class VariableServerListenThread_test : public ::testing::Test
{
protected:
    // Static global dependencies that I would like to eventually mock out
    Trick::CommandLineArguments cmd_args;
    Trick::VariableServer varserver;

    // Static global dependencies that I have mocked out
    MockExecutive executive;
    MockMessagePublisher message_publisher;

    MockTCPClientListener * listener;
    MockMulticastGroup * mcast;

    VariableServerListenThread_test()
    {
        // Set up dependencies that haven't been broken
        Trick::VariableServerSessionThread::set_vs_ptr(&varserver);

        // Set up mocks
        listener = new MockTCPClientListener;
        mcast = new MockMulticastGroup;
    }

    ~VariableServerListenThread_test() {}

    void SetUp() {}

    void TearDown() {}
};

void setup_normal_listener_expectations(MockTCPClientListener * listener)
{
    // Starting the connection succeeds
    EXPECT_CALL(*listener, getHostname()).WillRepeatedly(Return("MyHostname"));
    EXPECT_CALL(*listener, getPort()).WillRepeatedly(Return(1234));
}

void setup_normal_mcast_expectations(MockMulticastGroup * mcast)
{
    EXPECT_CALL(*mcast, initialize()).WillOnce(Return(0));

    EXPECT_CALL(*mcast, isInitialized()).WillRepeatedly(Return(1));

    EXPECT_CALL(*mcast, addAddress(_, _)).Times(2);

    EXPECT_CALL(*mcast, broadcast(_)).Times(AtLeast(1));
}

TEST_F(VariableServerListenThread_test, init_listen_device)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    EXPECT_CALL(*listener, initialize()).WillOnce(Return(0));

    Trick::VariableServerListenThread listen_thread(listener);

    // ACT
    listen_thread.init_listen_device();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, init_listen_device_fails)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    EXPECT_CALL(*listener, initialize()).WillOnce(Return(-1));

    Trick::VariableServerListenThread listen_thread(listener);

    // ACT
    int status = listen_thread.init_listen_device();

    // ASSERT
    EXPECT_EQ(status, -1);
}

TEST_F(VariableServerListenThread_test, get_hostname)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    Trick::VariableServerListenThread listen_thread(listener);

    // ACT
    const char * hostname = listen_thread.get_hostname();

    // ASSERT
    EXPECT_STREQ(hostname, "MyHostname");
}

TEST_F(VariableServerListenThread_test, get_port)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    Trick::VariableServerListenThread listen_thread(listener);

    // ACT
    int port = listen_thread.get_port();

    // ASSERT
    EXPECT_EQ(port, 1234);
}

TEST_F(VariableServerListenThread_test, get_port_returns_requested)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    Trick::VariableServerListenThread listen_thread(listener);

    // ACT
    listen_thread.set_port(4321);
    int port = listen_thread.get_port();

    // ASSERT
    EXPECT_EQ(port, 4321);
}

TEST_F(VariableServerListenThread_test, check_and_move_listen_device_init_fails)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_port(4321);

    EXPECT_CALL(*listener, disconnect());
    EXPECT_CALL(*listener, initialize(_, _)).WillOnce(Return(1));

    EXPECT_CALL(message_publisher, publish(MSG_ERROR, _));

    // ACT
    int status = listen_thread.check_and_move_listen_device();

    // ASSERT
    EXPECT_EQ(status, -1);
}

TEST_F(VariableServerListenThread_test, check_and_move_listen_device)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_port(4321);

    EXPECT_CALL(*listener, disconnect());
    EXPECT_CALL(*listener, initialize(_, _)).WillOnce(Return(0));

    // ACT
    int status = listen_thread.check_and_move_listen_device();

    // ASSERT
    EXPECT_EQ(status, 0);
}

TEST_F(VariableServerListenThread_test, run_thread)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    setup_normal_mcast_expectations(mcast);

    EXPECT_CALL(*listener, setBlockMode(true)).Times(1);

    EXPECT_CALL(*listener, checkForNewConnections()).WillRepeatedly(Return(false));

    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_multicast_group(mcast);

    // ACT
    listen_thread.create_thread();

    sleep(3);

    listen_thread.cancel_thread();
    listen_thread.join_thread();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, run_thread_no_broadcast)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    // Expect no calls to mcast
    EXPECT_CALL(*mcast, initialize()).Times(0);

    EXPECT_CALL(*mcast, broadcast(_)).Times(0);

    EXPECT_CALL(*listener, setBlockMode(true)).Times(1);

    EXPECT_CALL(*listener, checkForNewConnections()).WillRepeatedly(Return(false));

    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_broadcast(false);
    listen_thread.set_multicast_group(mcast);

    // ACT
    listen_thread.create_thread();

    sleep(3);

    listen_thread.cancel_thread();
    listen_thread.join_thread();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, run_thread_turn_on_broadcast)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);
    setup_normal_mcast_expectations(mcast);

    EXPECT_CALL(*listener, setBlockMode(true)).Times(1);

    EXPECT_CALL(*listener, checkForNewConnections()).WillRepeatedly(Return(false));

    EXPECT_CALL(*mcast, isInitialized()).WillOnce(Return(0)).WillRepeatedly(Return(1));

    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_broadcast(false);
    listen_thread.set_multicast_group(mcast);

    // ACT
    listen_thread.create_thread();

    EXPECT_EQ(listen_thread.get_broadcast(), false);

    sleep(3);
    listen_thread.set_broadcast(true);
    sleep(3);
    EXPECT_EQ(listen_thread.get_broadcast(), true);

    listen_thread.cancel_thread();
    listen_thread.join_thread();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, accept_connection)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    // Expect no calls to mcast
    EXPECT_CALL(*mcast, initialize()).Times(0);

    EXPECT_CALL(*mcast, broadcast(_)).Times(0);

    EXPECT_CALL(*listener, setBlockMode(true)).Times(1);

    EXPECT_CALL(*listener, checkForNewConnections()).WillOnce(Return(true)).WillRepeatedly(Return(false));

    MockTCPConnection connection;
    EXPECT_CALL(connection, start()).WillOnce(Return(0));

    EXPECT_CALL(connection, read(_, _)).WillOnce(Return(-1));

    EXPECT_CALL(connection, disconnect());

    EXPECT_CALL(*listener, setUpNewConnection()).WillOnce(Return(&connection));

    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_broadcast(false);
    listen_thread.set_multicast_group(mcast);

    // ACT
    listen_thread.create_thread();

    std::stringstream stream;
    listen_thread.dump(stream);
    ASSERT_TRUE(stream.str().size() > 0);

    sleep(3);

    listen_thread.cancel_thread();
    listen_thread.join_thread();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, connection_fails)
{
    // ARRANGE
    setup_normal_listener_expectations(listener);

    // Expect no calls to mcast
    EXPECT_CALL(*mcast, initialize()).Times(0);

    EXPECT_CALL(*mcast, broadcast(_)).Times(0);

    EXPECT_CALL(*listener, setBlockMode(true)).Times(1);

    EXPECT_CALL(*listener, checkForNewConnections()).WillOnce(Return(true)).WillRepeatedly(Return(false));

    MockTCPConnection connection;
    EXPECT_CALL(connection, start()).WillOnce(Return(-1));

    EXPECT_CALL(connection, disconnect());

    EXPECT_CALL(*listener, setUpNewConnection()).WillOnce(Return(&connection));

    Trick::VariableServerListenThread listen_thread(listener);
    listen_thread.set_broadcast(false);
    listen_thread.set_multicast_group(mcast);

    // ACT
    listen_thread.create_thread();

    sleep(3);

    listen_thread.cancel_thread();
    listen_thread.join_thread();

    // ASSERT
}

TEST_F(VariableServerListenThread_test, restart_fails)
{
    // ARRANGE

    setup_normal_listener_expectations(listener);
    Trick::VariableServerListenThread listen_thread(listener);

    EXPECT_CALL(*listener, restart());
    EXPECT_CALL(*listener, validateSourceAddress(_)).WillOnce(Return(false));

    EXPECT_CALL(*listener, initialize(_, _)).WillOnce(Return(-1));

    EXPECT_CALL(*listener, disconnect());

    EXPECT_CALL(message_publisher, publish(MSG_ERROR, _));
    EXPECT_CALL(message_publisher, publish(MSG_INFO, _));

    // ACT
    // ASSERT
    EXPECT_EQ(listen_thread.restart(), -1);
}
