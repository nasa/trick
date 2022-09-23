
#ifndef DATA_RECORD_PROTO_H
#define DATA_RECORD_PROTO_H

#ifdef __cplusplus
#include "trick/DataRecordGroup.hh"

extern "C" {
#endif

int dr_remove_files(void) ;
int dr_enable(void) ;
int dr_disable(void) ;
int dr_enable_group( const char * in_name ) ;
int dr_disable_group( const char * in_name ) ;
int dr_record_now_group( const char * in_name ) ;
int dr_set_max_file_size ( uint64_t bytes ) ;
void remove_all_data_record_groups(void) ;
int set_max_size_record_group (const char * in_name, uint64_t bytes ) ;


#ifdef __cplusplus
int add_data_record_group( Trick::DataRecordGroup * in_group, Trick::DR_Buffering buffering = Trick::DR_Not_Specified ) ;
int remove_data_record_group( Trick::DataRecordGroup * in_group ) ;
Trick::DataRecordGroup * get_data_record_group( std::string in_name ) ;
}
#endif

#endif

