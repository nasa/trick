
#include <iostream>
#include <string.h>

#include "trick/VariableServer.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/exec_proto.h"
#include "trick/realtimesync_proto.h"

int Trick::VariableServerThread::copy_data_freeze() {

    int ret = 0 ;
    long long curr_frame = exec_get_freeze_frame_count() ;
    long long temp_frame  ;

    if ( enabled and copy_mode == VS_COPY_TOP_OF_FRAME) {
        temp_frame = curr_frame % freeze_frame_multiple ;
        if ( temp_frame == freeze_frame_offset ) {
            copy_sim_data() ;
            if ( !pause_cmd and write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    exit_cmd = true ;
                }
            }
        }
    }
    return ret ;
}

int Trick::VariableServerThread::copy_data_freeze_scheduled(long long curr_tics) {

    int ret = 0 ;

    if ( enabled and copy_mode == VS_COPY_SCHEDULED) {
        if ( freeze_next_tics <= curr_tics ) {
            copy_sim_data() ;
            if ( !pause_cmd and write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    exit_cmd = true ;
                }
            }
            freeze_next_tics = curr_tics + cycle_tics ;
        }
    }
    return ret ;
}

int Trick::VariableServerThread::copy_data_scheduled(long long curr_tics) {

    int ret = 0 ;

    if ( enabled and copy_mode == VS_COPY_SCHEDULED) {
        if ( next_tics <= curr_tics ) {
            copy_sim_data() ;
            if ( !pause_cmd and write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    exit_cmd = true ;
                }
            }
            next_tics = curr_tics + cycle_tics ;
        }
    }
    return ret ;
}

int Trick::VariableServerThread::copy_data_top() {

    int ret = 0 ;
    long long curr_frame = exec_get_frame_count() ;
    long long temp_frame  ;

    if ( enabled and copy_mode == VS_COPY_TOP_OF_FRAME) {
        temp_frame = curr_frame % frame_multiple ;
        if ( temp_frame == frame_offset ) {
            copy_sim_data() ;
            if ( !pause_cmd and write_mode == VS_WRITE_WHEN_COPIED and is_real_time()) {
                ret = write_data() ;
                if ( ret < 0 ) {
                    exit_cmd = true ;
                }
            }
        }
    }
    return ret ;
}
