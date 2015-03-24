
#include <iostream>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/CheckPointRestart/include/checkpoint_stl.hh"

int Trick::Executive::post_checkpoint() {

    TMM_delete_var_a(all_jobs_for_checkpoint) ;

    all_jobs_for_checkpoint = NULL ;
    delete_stl(sim_objects , std::string("trick_sys") , std::string("sim_objects")) ;

    if ( ! freeze_times.empty() ) {
        delete_stl(freeze_times , std::string("trick_sys") , std::string("freeze_times")) ;
    }

    return(0) ;

}
