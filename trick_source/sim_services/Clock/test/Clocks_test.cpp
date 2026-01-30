#include <iostream>
#include <signal.h>
#include <sys/types.h>

#include "trick/Clock.hh"
#include "trick/GetTimeOfDayClock.hh"
#include "trick/JobData.hh"
#include "trick/clock_proto.h"
#include "gtest/gtest.h"

namespace Trick
{

class ClocksTest : public ::testing::Test
{
protected:
    Trick::GetTimeOfDayClock timclk;

    GetTimeOfDayClockTest() {}

    ~GetTimeOfDayClockTest() {}

    virtual void SetUp() {}

    virtual void TearDown() {}
};

TEST_F(ClocksTest, ExampleTest) {}

} // namespace Trick
