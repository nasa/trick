
#ifndef EXEC_PROTO_HH
#define EXEC_PROTO_HH

#include "trick/Executive.hh"
#include "trick/SimObject.hh"

Trick::Executive * exec_get_exec_cpp() ;

int exec_add_sim_object( Trick::SimObject * , const char * = NULL ) ;
int exec_remove_sim_object( Trick::SimObject * ) ;

int exec_get_sim_objects(std::vector<Trick::SimObject *>& in_sim_object_vector ) ;

Trick::JobData * exec_get_job(const char * job_name, unsigned int j_instance = 1 ) ;
Trick::JobData * exec_get_curr_job() ;

Trick::Threads * exec_get_thread( unsigned int thread_id ) ;

int exec_get_all_jobs_vector(std::vector<Trick::JobData *> & in_jobs_vector) ;

int exec_register_scheduler( Trick::Scheduler * scheduler ) ;

#endif
