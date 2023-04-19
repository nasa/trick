#ifndef MOCK_REALTIME_SYNC_HH
#define MOCK_REALTIME_SYNC_HH

#include "trick/GetTimeOfDayClock.hh"
#include "trick/RealtimeSync.hh"
#include <gmock/gmock.h>

// This sucks but refactoring this is out of the scope for now
Trick::GetTimeOfDayClock& my_static_clock() {
    static Trick::GetTimeOfDayClock clock;
    return clock;
}

class MockRealtimeSync : public Trick::RealtimeSync {
    public: 
        MockRealtimeSync() : RealtimeSync(&(my_static_clock())) {}

        MOCK_METHOD0(is_active, bool());
};

#endif