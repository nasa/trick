
#ifndef DATA_RECORD_PROTO_H
#define DATA_RECORD_PROTO_H

#ifdef __cplusplus
#include "sim_services/DataRecord/include/DataRecordGroup.hh"

extern "C" {
#endif

int dr_remove_files() ;
int dr_enable() ;
int dr_disable() ;
int dr_enable_group( const char * in_name ) ;
int dr_disable_group( const char * in_name ) ;
int dr_record_now_group( const char * in_name ) ;

#ifdef __cplusplus
int add_data_record_group( Trick::DataRecordGroup * in_group, Trick::DR_Buffering buffering = Trick::DR_Not_Specified ) ;
int remove_data_record_group( Trick::DataRecordGroup * in_group ) ;
Trick::DataRecordGroup * get_data_record_group( std::string in_name ) ;
}
#endif

#endif

