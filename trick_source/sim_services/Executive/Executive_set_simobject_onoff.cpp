
#include <iostream>

#include "trick/Executive.hh"

int Trick::Executive::set_sim_object_jobs_onoff(std::string sim_object_name, int on) {
    for ( int ii = 0 ; ii < sim_objects.size() ; ii++ ) {
        if ( ! sim_objects[ii]->name.compare(sim_object_name) ) {
            if (on) {
                sim_objects[ii]->enable_all_jobs();
            } else {
                sim_objects[ii]->disable_all_jobs();
            }

            return 0;
        }
    }

    return -1;
}


int Trick::Executive::set_sim_object_onoff(std::string sim_object_name, int on) {

    SimObject * so ;
    unsigned int ii , jj ;

    for ( ii = 0 ; ii < sim_objects.size() ; ii++ ) {
        if ( ! sim_objects[ii]->name.compare(sim_object_name) ) {

            so = sim_objects[ii] ;

            so->object_disabled = !on;
            if (on) {
                // Apply saved state
                for ( jj = 0 ; jj < so->jobs.size() ; jj++ ) {
                    std::map<std::string, bool>::iterator saved_state_it = so->saved_job_states.find(so->jobs[jj]->name);
                    if (saved_state_it != so->saved_job_states.end()) {
                        so->jobs[jj]->disabled = saved_state_it->second;
                    } else {
                        // If job is not in map, turn it on
                        so->jobs[jj]->disabled = false;
                    }
                }
                so->saved_job_states.clear();
            } else {
                for ( jj = 0 ; jj < so->jobs.size() ; jj++ ) {
                    // Save state, turn off
                    so->saved_job_states[so->jobs[jj]->name] = so->jobs[jj]->disabled;
                    so->jobs[jj]->disabled = true ;
                }
            }
            return(0) ;
        }
    }
    return(-1) ;
}

int Trick::Executive::get_sim_object_onoff (std::string sim_object_name) {
    for ( int ii = 0 ; ii < sim_objects.size() ; ii++ ) {
        if ( ! sim_objects[ii]->name.compare(sim_object_name) ) {
            return !sim_objects[ii]->object_disabled ;
        }
    }
    return -1;
}

