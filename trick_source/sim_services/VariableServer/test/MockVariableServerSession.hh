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
        MOCK_CONST_METHOD0(get_write_mode, VS_WRITE_MODE());
        MOCK_CONST_METHOD0(get_copy_mode, VS_COPY_MODE());
        MOCK_METHOD0(set_log_on, int());
        MOCK_METHOD0(set_log_off, int());
        MOCK_METHOD0(copy_sim_data, int());
        MOCK_CONST_METHOD0(get_update_rate, double());
};

#endif