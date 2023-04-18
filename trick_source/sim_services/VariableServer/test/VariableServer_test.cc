/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServer class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>


#include "trick/VariableServer.hh"
#include "trick/Mock/MockVariableServerSession.hh"


/*
 Test Fixture.
 */
class VariableServer_test : public ::testing::Test {
	protected:
        Trick::VariableServer vs;

		VariableServer_test() { 

        }

		~VariableServer_test() {
        }

		void SetUp() {}
		void TearDown() {}
};

TEST_F(VariableServer_test, set_log_on) {
    // ARRANGE
    MockVariableServerSession session;
    EXPECT_CALL(session, set_log_on())
        .Times(1);

    pthread_t id = (pthread_t) 5;
    vs.add_session(id, &session);

    // ACT
    vs.set_var_server_log_on();

    // ASSERT
    EXPECT_EQ(vs.get_log(), true);
}

TEST_F(VariableServer_test, set_log_off) {
    // ARRANGE
    MockVariableServerSession session;
    EXPECT_CALL(session, set_log_off())
        .Times(1);

    pthread_t id = (pthread_t) 5;
    vs.add_session(id, &session);

    // ACT
    vs.set_var_server_log_off();

    // ASSERT
    EXPECT_EQ(vs.get_log(), false);
}

TEST_F(VariableServer_test, enabled_by_default) {
    // ARRANGE
    // ACT
    // ASSERT
    EXPECT_EQ(vs.get_enabled(), true);
}

TEST_F(VariableServer_test, set_enabled) {
    // ARRANGE
    // ACT
    vs.set_enabled(false);

    // ASSERT
    EXPECT_EQ(vs.get_enabled(), false);
}

TEST_F(VariableServer_test, info_msg_off_by_default) {
    // ARRANGE
    // ACT
    // ASSERT
    EXPECT_EQ(vs.get_info_msg(), false);
}

TEST_F(VariableServer_test, set_info_msg) {
    // ARRANGE
    // ACT
    vs.set_var_server_info_msg_on();

    // ASSERT
    EXPECT_EQ(vs.get_info_msg(), true);
}

TEST_F(VariableServer_test, set_info_msg_off) {
    // ARRANGE
    // ACT
    vs.set_var_server_info_msg_off();

    // ASSERT
    EXPECT_EQ(vs.get_info_msg(), false);
}

TEST_F(VariableServer_test, info_dump) {

    MockVariableServerSession session;
    pthread_t id = (pthread_t) 5;
    vs.add_session(id, &session);

    std::cout << vs << std::endl;

}