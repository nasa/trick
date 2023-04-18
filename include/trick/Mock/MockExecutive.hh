#ifndef MOCK_EXECUTIVE_HH
#define MOCK_EXECUTIVE_HH

#include "trick/Executive.hh"
#include <gmock/gmock.h>

class MockExecutive : public Trick::Executive {
    public: 
        MOCK_METHOD0(get_time_tics, long long());
        MOCK_METHOD0(get_sim_time, double());
};

#endif