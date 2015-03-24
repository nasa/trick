
#include "sim_services/MonteCarlo/include/MonteCarlo.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

/** @par Detailed Design: */
void Trick::MonteCarlo::dryrun() {

    /** <ul><li> For all runs: run the pre run jobs. */
    MonteRun * curr_run;
    while ((curr_run = get_next_dispatch())) {
        prepare_run(curr_run);
    }

    /** <ul><li> Run the master shutdown jobs */
    run_queue(&master_shutdown_queue, "in master_shutdown queue") ;

    message_publish(MSG_INFO, "Monte [Master] Dry run complete.\n") ;

    exit(0) ;
}
