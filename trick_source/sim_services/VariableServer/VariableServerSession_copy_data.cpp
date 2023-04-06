
#include <iostream>
#include <string.h>

#include "trick/VariableServerSession.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/exec_proto.h"
#include "trick/realtimesync_proto.h"

// These methods should be called from approprate jobs or from the VST

int Trick::VariableServerSession::copy_data_freeze() {
    int ret = 0 ;

    if (!_enabled)
        return ret;

    if (_copy_mode == VS_COPY_TOP_OF_FRAME) {
        long long curr_frame = exec_get_freeze_frame_count() ;
        long long temp_frame = curr_frame % _freeze_frame_multiple ;

        if ( temp_frame == _freeze_frame_offset ) {
            copy_sim_data() ;
            if ( !_pause_cmd and _write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    _exit_cmd = true ;
                }
            }
        }
    }

    return ret ;
}

int Trick::VariableServerSession::copy_data_freeze_scheduled(long long curr_tics) {
    int ret = 0 ;

    if (!_enabled)
        return ret;

    if (_copy_mode == VS_COPY_SCHEDULED) {
        if ( _freeze_next_tics <= curr_tics ) {
            copy_sim_data() ;
            if ( !_pause_cmd && _write_mode == VS_WRITE_WHEN_COPIED && is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    _exit_cmd = true ;
                }
            }
            _freeze_next_tics = curr_tics + _cycle_tics ;
        }
    }
    return ret ;
}

int Trick::VariableServerSession::copy_data_scheduled(long long curr_tics) {
    int ret = 0 ;

    if (!_enabled)
        return ret;

    if (_copy_mode == VS_COPY_SCHEDULED) {
        if ( _next_tics <= curr_tics ) {
            copy_sim_data() ;
            if ( !_pause_cmd and _write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    _exit_cmd = true ;
                }
            }
            _next_tics = curr_tics + _cycle_tics ;
        }
    }
    return ret ;
}

int Trick::VariableServerSession::copy_data_top() {
    int ret = 0 ;

    if (!_enabled)
        return ret;

    if (_copy_mode == VS_COPY_TOP_OF_FRAME) {
        long long curr_frame = exec_get_frame_count() ;
        long long temp_frame = curr_frame % _frame_multiple ;
        if ( temp_frame == _frame_offset ) {
            copy_sim_data() ;
            if ( !_pause_cmd && _write_mode == VS_WRITE_WHEN_COPIED && is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    _exit_cmd = true ;
                }
            }
        }
    }
    return ret ;
}

int Trick::VariableServerSession::copy_data_async() {
    int ret = 0;

    if (!_enabled) 
        return ret;

    if (_copy_mode == VS_COPY_ASYNC ) {
        copy_sim_data() ;
    }

    // Write data out to connection if async mode or non-realtime, and not paused
    bool should_write_async = (_write_mode == VS_WRITE_ASYNC) || 
                                ( _copy_mode == VS_COPY_ASYNC &&  _write_mode == VS_WRITE_WHEN_COPIED)|| 
                                (! is_real_time());

    if ( !_pause_cmd && should_write_async) {
        ret = write_data() ;
        if ( ret < 0 ) {
            _exit_cmd = true ;
        }
    }

    return ret;
}
