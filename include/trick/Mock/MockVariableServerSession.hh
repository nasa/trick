#ifndef MOCK_VARIABLE_SERVER_SESSION_HH
#define MOCK_VARIABLE_SERVER_SESSION_HH

#include "trick/VariableServerSession.hh"
#include <gmock/gmock.h>

class MockVariableServerSession : public Trick::VariableServerSession {
    public: 
        MOCK_METHOD0(handle_message, int());
        MOCK_METHOD0(write_data, int());
        MOCK_METHOD0(get_exit_cmd, bool());
        MOCK_METHOD0(get_pause, bool());
        MOCK_CONST_METHOD0(get_enabled, bool());
        MOCK_CONST_METHOD0(get_write_mode, VS_WRITE_MODE());
        MOCK_CONST_METHOD0(get_copy_mode, VS_COPY_MODE());
        MOCK_METHOD1(set_log, int(bool on_off));
        MOCK_METHOD1(set_session_log, int(bool on_off));
        MOCK_METHOD0(copy_sim_data, int());
        MOCK_CONST_METHOD0(get_update_rate, double());
        MOCK_CONST_METHOD0(get_frame_multiple, int());
        MOCK_CONST_METHOD0(get_frame_offset, int());
        MOCK_CONST_METHOD0(get_freeze_frame_multiple, int());
        MOCK_CONST_METHOD0(get_freeze_frame_offset, int());
        MOCK_CONST_METHOD0(get_next_tics, long long ());
        MOCK_CONST_METHOD0(get_freeze_next_tics, long long());
        MOCK_METHOD0(set_exit_cmd, void());

        MOCK_METHOD0(copy_and_write_async, int());

        // Accessor for the concrete version
        int copy_and_write_async_concrete() { return Trick::VariableServerSession::copy_and_write_async(); }
};

#endif