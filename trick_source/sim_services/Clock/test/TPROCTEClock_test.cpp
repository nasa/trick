
/******************NOTES**********************************************
 * The TPRO CTE clock requires special hardware to work. These tests
 * ensure that the TPRO class properly handles cases in which no
 * hardware is in use.
 *
 * Listed requirements are under requirement Trick-153 (3.7.1.2)
 *         Real-time control shall be able to use an external clock source
*********************************************************************/



#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <map>


#include "gtest/gtest.h"
#include "trick/Clock.hh"
#include "trick/clock_proto.h"
#include "trick/GetTimeOfDayClock.hh"
#include "trick/TPROCTEClock.hh"
#include "trick/BC635Clock.hh"
#include "trick/JobData.hh"


// Stub for message_publish
extern "C" int message_publish(int level, const char * format_msg, ...) { (void)level; (void)format_msg; return 0; }

class TPROCTEClockTest : public ::testing::Test {

    protected:
        Trick::TPROCTEClock proclk;

        TPROCTEClockTest() {}
        ~TPROCTEClockTest() {}
        virtual void SetUp() {}
        virtual void TearDown() {}

        //Trick::RequirementScribe req;
} ;

//std::map< std::string , unsigned int > TPROCTEClockTest::num_reqs;

/* Ensure clock initializes correctly */
TEST_F(TPROCTEClockTest, Initialize) {
    //"The TPROCTE clock shall initialize reference time of 0, and all ratios set to 1.");

    /* General expected clock initialization */
    EXPECT_EQ(proclk.get_rt_clock_ratio() , 1.0);
    EXPECT_EQ(proclk.sim_tic_ratio, 1.0);
    EXPECT_EQ(proclk.ref_time_tics, 0);

    /* TPROCTEClock initialization tests */
    EXPECT_STREQ(proclk.get_name() , "TPROCTE");

}

/* Run tests for when no hardware is available */
#ifndef _TPRO_CTE
TEST_F(TPROCTEClockTest, NoHardware) {
    //"The TPROCTE clock shall display error messages when attempting to use its functions with no hardware");

    long long req_time = rand();

    EXPECT_EQ(proclk.clock_init(), -1);
    EXPECT_EQ(proclk.wall_clock_time(), 0);
    EXPECT_EQ(proclk.clock_spin(req_time), req_time);
    EXPECT_EQ(proclk.clock_stop(), 0);

    proclk.set_reference(req_time);
    EXPECT_EQ(proclk.clock_time(), -req_time);

    proclk.adjust_ref_time(req_time);
    EXPECT_EQ(proclk.clock_time(), -2*req_time);


}

#endif

/* Ensure generic clock functions work properly */
TEST_F(TPROCTEClockTest, TestRefTimes) {
    //"The TPROCTE clock shall provide the ability to adjust the reference time");

    long long secs_remainder;
    int tics_per_s = 1000000000;
    double align_tics_mult = 1.5;

    int clock_unit = int((tics_per_s * align_tics_mult)/(proclk.rt_clock_ratio * proclk.sim_tic_ratio));

    proclk.sync_to_wall_clock(align_tics_mult, tics_per_s);
    secs_remainder = proclk.ref_time_tics% clock_unit;
    EXPECT_EQ(secs_remainder, 0);

}


