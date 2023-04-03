/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServer class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockVariableServerSession.hh"

#include "trick/VariableServer.hh"


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