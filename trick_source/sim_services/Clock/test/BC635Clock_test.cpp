
/******************NOTES**********************************************
 * The BC635 clock requires special hardware to work. These tests
 * ensure that the TPRO class properly handles cases in which no
 * hardware is in use.
 *
 * Listed requirements are under requirement Trick-153 (3.7.1.2)
 * Real-time control shall be able to use an external clock source
 *********************************************************************/


#include <iostream>
#include <sys/types.h>
#include <signal.h>

#include "gtest/gtest.h"
#include "trick/Clock.hh"
#include "trick/clock_proto.h"
#include "trick/GetTimeOfDayClock.hh"
#include "trick/TPROCTEClock.hh"
#include "trick/BC635Clock.hh"
#include "trick/JobData.hh"

// Stub for message_publish
extern "C" int message_publish(int level, const char * format_msg, ...) { (void)level; (void)format_msg; return 0; }

class BC635ClockTest : public ::testing::Test {

	protected:
		Trick::BC635Clock bcclk;

		BC635ClockTest() {}
		~BC635ClockTest() {}
		virtual void SetUp() {}
		virtual void TearDown() {}

} ;

/* Ensure clock initializes correctly */
TEST_F(BC635ClockTest, Initialize) {

	// "The BC635 clock shall initialize reference time of 0, and all ratios set to 1.");

	/* General expected clock initialization */
	EXPECT_EQ(bcclk.get_rt_clock_ratio() , 1.0);
	EXPECT_EQ(bcclk.sim_tic_ratio, 1.0);
	EXPECT_EQ(bcclk.ref_time_tics, 0);

	/* BC635 clock initialization tests */
	EXPECT_STREQ(bcclk.get_name() , "BC635");
}


/* Run tests for when no hardware is available */
#ifndef _BC635
TEST_F(BC635ClockTest, ErrorMessages) {

	//"The BC635 clock shall display error messages when attempting to use its functions with no hardware.");
	/* curr_time = 0 always */

	long long req_time = rand();

	EXPECT_EQ(bcclk.clock_init(), -1);
	EXPECT_EQ(bcclk.wall_clock_time(), 0);
	EXPECT_EQ(bcclk.clock_spin(req_time), req_time);
	EXPECT_EQ(bcclk.clock_stop(), 0);

    bcclk.set_reference(req_time);
    EXPECT_EQ(bcclk.clock_time(), -req_time);

    bcclk.adjust_ref_time(req_time);
    EXPECT_EQ(bcclk.clock_time(), -2*req_time);

}

/* Ensure generic clock functions still work when no hardware is available */
TEST_F(BC635ClockTest, TestRefTimes) {
	//"The BC635 clock shall provide the ability to adjust the reference time");

	long long secs_remainder;
	int tics_per_s = 1000000000;
	double align_tics_mult = 1.5;

	int clock_unit = int((tics_per_s * align_tics_mult)/(bcclk.rt_clock_ratio * bcclk.sim_tic_ratio));

    bcclk.sync_to_wall_clock(align_tics_mult, tics_per_s);
	secs_remainder = bcclk.ref_time_tics% clock_unit;
	EXPECT_EQ(secs_remainder, 0);
}
#endif
