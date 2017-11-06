
#include <iostream>

#include "trick/Executive.hh"

int Trick::Executive::set_sim_object_onoff(std::string sim_object_name, int on) {

    SimObject * so ;
    unsigned int ii , jj ;

    for ( ii = 0 ; ii < sim_objects.size() ; ii++ ) {
        if ( ! sim_objects[ii]->name.compare(sim_object_name) ) {
            so = sim_objects[ii] ;
            for ( jj = 0 ; jj < so->jobs.size() ; jj++ ) {
                so->jobs[jj]->disabled = !on ;
            }
            return(0) ;
        }
    }
    return(-1) ;

}

