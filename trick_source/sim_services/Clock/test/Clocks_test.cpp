#include <iostream>
#include <sys/types.h>
#include <signal.h>
 
#include "gtest/gtest.h"
#include "sim_services/Clock/include/Clock.hh"
#include "sim_services/Clock/include/clock_proto.h"
#include "sim_services/Clock/include/GetTimeOfDayClock.hh"
#include "sim_services/Clock/include/TPROCTEClock.hh"
#include "sim_services/Clock/include/BC635Clock.hh"
#include "sim_services/SimObject/include/JobData.hh"

namespace Trick {

class ClocksTest : public ::testing::Test {
	
	protected:
		Trick::GetTimeOfDayClock timclk;

		GetTimeOfDayClockTest() {}
		~GetTimeOfDayClockTest() {}
		virtual void SetUp() {}
		virtual void TearDown() {}
} ;

TEST_F(ClocksTest, ExampleTest) {

}


}

