#include "gtest/gtest.h"
#include "trick/RealtimeSync.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"

namespace {

struct Terminated {};
SIM_MODE mode = Run;
Trick::RealtimeSync * monitored_sync = NULL;
double action_peak = 0.0;
unsigned int action_overruns = 0;
int freeze_calls = 0;

void record_overrun_action() {
    action_peak = monitored_sync->peak_frame_overrun;
    action_overruns = monitored_sync->total_overrun;
}

class TestClock : public Trick::Clock {
public:
    TestClock() : Clock(1000, "test"), now(0) {}
    long long now;
    int clock_init() override { return 0; }
    int clock_stop() override { return 0; }
    long long wall_clock_time() override { return now; }
    long long clock_time() override { return now; }
    long long clock_reset(long long ref) override { now = ref; return ref; }
    long long clock_spin(long long target) override {
        if (now < target) { now = target; }
        return now;
    }
};

class TestTimer : public Trick::Timer {
public:
    int init() override { return 0; }
    int start(double) override { return 0; }
    int reset(double) override { return 0; }
    int stop() override { return 0; }
    int pause() override { return 0; }
    int shutdown() override { return 0; }
};

class RealtimeSyncTest : public ::testing::Test {
protected:
    TestClock clock;
    TestTimer timer;
    Trick::RealtimeSync sync;

    RealtimeSyncTest() : sync(&clock, &timer) {}

    void SetUp() override {
        mode = ::Run;
        monitored_sync = &sync;
        action_peak = 0.0;
        action_overruns = 0;
        freeze_calls = 0;
        // Existing counters normally receive simulation default initialization.
        sync.total_overrun = 0;
        sync.frame_overrun_cnt = 0;
        sync.last_clock_time = 0;
        ASSERT_EQ(0, sync.initialize());
    }

    void TearDown() override { monitored_sync = NULL; }

    void activate() {
        sync.enable();
        sync.initialize();
        sync.start_realtime(0.1, 0);
    }
};

TEST_F(RealtimeSyncTest, StartsWithZeroHealth) {
    EXPECT_EQ(0ULL, sync.completed_frame_count);
    EXPECT_DOUBLE_EQ(0.0, sync.peak_frame_overrun);
}

TEST_F(RealtimeSyncTest, NonRealtimeFramesCountWithoutRecordingLateness) {
    clock.now = 500;
    ASSERT_EQ(0, sync.rt_monitor(100));
    ASSERT_EQ(0, sync.rt_monitor(200));
    EXPECT_EQ(2ULL, sync.completed_frame_count);
    EXPECT_DOUBLE_EQ(0.0, sync.peak_frame_overrun);
    EXPECT_EQ(0U, sync.total_overrun);
}

TEST_F(RealtimeSyncTest, PeakAndTotalOverrunsSurviveSubsequentHealthyFrames) {
    activate();
    clock.now = 350;
    ASSERT_EQ(0, sync.rt_monitor(100));
    EXPECT_DOUBLE_EQ(0.25, sync.peak_frame_overrun);
    clock.now = 400;
    ASSERT_EQ(0, sync.rt_monitor(300));
    clock.now = 450;
    ASSERT_EQ(0, sync.rt_monitor(500));
    EXPECT_LT(sync.frame_overrun, 0.0);
    EXPECT_DOUBLE_EQ(0.25, sync.peak_frame_overrun);
    EXPECT_EQ(2U, sync.total_overrun);
    EXPECT_EQ(0U, sync.frame_overrun_cnt);
    EXPECT_EQ(3ULL, sync.completed_frame_count);
    clock.now = 1100;
    ASSERT_EQ(0, sync.rt_monitor(600));
    EXPECT_DOUBLE_EQ(0.5, sync.peak_frame_overrun);
    EXPECT_EQ(4ULL, sync.completed_frame_count);
}

TEST_F(RealtimeSyncTest, OnTimeFrameHasZeroPeak) {
    activate();
    clock.now = 100;
    ASSERT_EQ(0, sync.rt_monitor(100));
    EXPECT_EQ(1ULL, sync.completed_frame_count);
    EXPECT_DOUBLE_EQ(0.0, sync.peak_frame_overrun);
}

TEST_F(RealtimeSyncTest, RetainsFinalOverrunBeforeTerminationWithoutCountingCompletion) {
    activate();
    sync.rt_max_overrun_cnt = 1;
    clock.now = 350;
    EXPECT_THROW(sync.rt_monitor(100), Terminated);
    EXPECT_DOUBLE_EQ(0.25, action_peak);
    EXPECT_EQ(1U, action_overruns);
    EXPECT_DOUBLE_EQ(0.25, sync.peak_frame_overrun);
    EXPECT_EQ(0ULL, sync.completed_frame_count);
}

TEST_F(RealtimeSyncTest, RetainsOverrunBeforeFreezeAndDoesNotCountFreezeFrames) {
    activate();
    sync.rt_max_overrun_time_tics = 200;
    sync.rt_overrun_freeze = true;
    clock.now = 350;
    ASSERT_EQ(0, sync.rt_monitor(100));
    EXPECT_EQ(1, freeze_calls);
    EXPECT_DOUBLE_EQ(0.25, action_peak);
    EXPECT_EQ(1U, action_overruns);
    EXPECT_TRUE(sync.freeze_shutdown);
    EXPECT_EQ(1ULL, sync.completed_frame_count);
    sync.freeze_init(0.1);
    sync.freeze_pause(0.1);
    EXPECT_EQ(1ULL, sync.completed_frame_count);
    EXPECT_DOUBLE_EQ(0.25, sync.peak_frame_overrun);
}

TEST_F(RealtimeSyncTest, EnableDisableAndRestartDoNotResetHealth) {
    activate();
    clock.now = 350;
    sync.rt_monitor(100);
    sync.disable();
    clock.now = 400;
    sync.rt_monitor(500);
    EXPECT_FALSE(sync.active);
    sync.freeze_init(0.1);
    sync.freeze_pause(0.1);
    sync.enable();
    sync.rt_monitor(600);
    EXPECT_TRUE(sync.active);
    sync.restart(600);
    mode = Freeze;
    sync.restart(600);
    EXPECT_EQ(3ULL, sync.completed_frame_count);
    EXPECT_DOUBLE_EQ(0.25, sync.peak_frame_overrun);
    EXPECT_EQ(1U, sync.total_overrun);
}

} // namespace

// Isolate the monitor from the Executive and message service, as in Clock tests.
extern "C" {
int exec_get_time_tic_value() { return 1000; }
long long exec_get_time_tics() { return 0; }
long long exec_get_freeze_time_tics() { return 0; }
double exec_get_software_frame() { return 0.1; }
double exec_get_freeze_frame() { return 0.1; }
SIM_MODE exec_get_mode() { return mode; }
int exec_get_rt_nap() { return 0; }
int exec_freeze() {
    record_overrun_action();
    ++freeze_calls;
    mode = Freeze;
    return 0;
}
int exec_terminate_with_return(int, const char *, int, const char *) {
    record_overrun_action();
    throw Terminated();
}
int message_publish(int, const char *, ...) { return 0; }
}
