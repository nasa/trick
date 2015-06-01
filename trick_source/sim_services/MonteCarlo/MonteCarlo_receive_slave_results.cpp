
#include "trick/MonteCarlo.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/tc_proto.h"

/**
 * @par Detailed Design:
 * This function performs a non-blocking accept on the listening socket.
 */
void Trick::MonteCarlo::receive_slave_results() {
    Trick::MonteSlave *curr_slave = NULL;
    int exit_status;
    unsigned int curr_slave_id;

    /** <ul><li> While there are slaves waiting to connect: */
    tc_accept(&listen_device, &connection_device);

    /** <ul><li> Read the id of incoming slave. */
    tc_read(&connection_device, (char*)&curr_slave_id, (int)sizeof(curr_slave_id));
    curr_slave_id = ntohl(curr_slave_id);
    curr_slave = get_slave(curr_slave_id);
    if (curr_slave == NULL) {
        message_publish(MSG_ERROR, "Monte [Master] the slave returned an invalid slave id\n") ;
    }
    /**
     * <li> If the slave is in the INITIALIZING state, it will send us the
     * machine name and port over which it is listening for new runs. 
     */
    if (curr_slave->state == MonteSlave::INITIALIZING) {
        if (verbosity >= ALL) {
            message_publish(MSG_INFO, "Monte [Master] Receiving initialization information from %s:%d.\n", 
                            curr_slave->machine_name.c_str(), curr_slave->id) ;
        }
        read_machine_name(curr_slave) ;
        read_slave_port(curr_slave) ;
        /** <li> Slave state is not INITIALIZING */
    } else {
        if (verbosity >= INFORMATIONAL) {
            message_publish(MSG_INFO, "Monte [Master] Receiving results for run %d from %s:%d.\n",
                 curr_slave->current_run->id, curr_slave->machine_name.c_str(), curr_slave->id) ;
        }
        /** <li> Otherwise, it is sending us the exit status for its current run. */
        tc_read(&connection_device, (char*)&exit_status, (int)sizeof(exit_status));
        exit_status = ntohl(exit_status);
        tc_disconnect(&connection_device);
        /**
         * <ul><li> This run may have already been resolved by another curr_slave if 
         * this curr_slave was marked as having timed out. If that is the case,
         * discard these results.
         */
        if (curr_slave->current_run->exit_status != MonteRun::INCOMPLETE) {
            if (verbosity >= ALL) {
                message_publish(MSG_INFO, "Monte [Master] Run %d has already been resolved. Discarding results.\n",
                                curr_slave->current_run->id) ;
            }
        /** <li> Otherwise, check the exit status: */
        } else {
            switch (exit_status) {
                /** <ul><li> Unkown errors, core dumps, and 
                  * successfully completed runs are not redispatched. 
                  */
                default:
                    if (verbosity >= ERROR) {
                        message_publish(MSG_ERROR, "Monte [Master] %s:%d reported unrecognized exit status %d\n"
                                                   "for run %d. Skipping.\n",
                                        curr_slave->machine_name.c_str(), curr_slave->id, exit_status, curr_slave->current_run->id) ;
                    }
                    resolve_run(curr_slave, MonteRun::UNKNOWN);
                break;
                case MonteRun::CORED:
                    if (verbosity >= ERROR) {
                        message_publish(MSG_ERROR, "Monte [Master] %s:%d reported core dump for run %d. Skipping.\n",
                                        curr_slave->machine_name.c_str(), curr_slave->id, curr_slave->current_run->id) ;
                    } 
                    resolve_run(curr_slave, MonteRun::CORED);
                break;
                case MonteRun::COMPLETE:
                    resolve_run(curr_slave, MonteRun::COMPLETE);
                break;
                /**
                 * <li> Timeouts and permission errors are redispatched. However, 
                 * we must first check to see if this run has already been processed
                 * in #check_timeouts, which can occur when the master determines
                 * that a curr_slave has timed out, and then that curr_slave itself reports 
                 * a timeout or permission error. </ul>
                 */
                case MonteRun::TIMEDOUT:
                    if (verbosity >= ERROR) {
                        message_publish(MSG_ERROR, "Monte [Master] %s:%d reported a timeout for run %d.\n",
                                        curr_slave->machine_name.c_str(), curr_slave->id, curr_slave->current_run->id) ;
                    }
                    if (!(curr_slave->state == MonteSlave::UNRESPONSIVE_RUNNING ||
                          curr_slave->state == MonteSlave::UNRESPONSIVE_STOPPING)) 
                    {
                        handle_retry(curr_slave, MonteRun::TIMEDOUT);
                    }
                break;
                case MonteRun::NO_PERM:
                    if (verbosity >= ERROR) {
                        message_publish(MSG_ERROR, "Monte [Master] %s:%d reported a failure to create output directories for run %d.\n",
                                        curr_slave->machine_name.c_str(), curr_slave->id, curr_slave->current_run->id) ;
                    }
                    if (!(curr_slave->state == MonteSlave::UNRESPONSIVE_RUNNING ||
                          curr_slave->state == MonteSlave::UNRESPONSIVE_STOPPING)) 
                    {
                        handle_retry(curr_slave, MonteRun::NO_PERM);
                    }
                break;
            }
        } // end of checking curr_slaves exit_status
        /** <li> Update the curr_slave's state. */
        if (curr_slave->state == MonteSlave::RUNNING || curr_slave->state == MonteSlave::UNRESPONSIVE_RUNNING) {
            curr_slave->state = MonteSlave::READY;
        } else if (curr_slave->state == MonteSlave::STOPPING || curr_slave->state == MonteSlave::UNRESPONSIVE_STOPPING) {
            curr_slave->state = MonteSlave::STOPPED;
        }
    }  // end of Slave state is not INITIALIZING
}

void Trick::MonteCarlo::read_slave_port(Trick::MonteSlave *curr_slave) {
    int bytes_read ;
    bytes_read = tc_read(&connection_device, (char *)&curr_slave->port, (int)sizeof(curr_slave->port));
    if (bytes_read == (int)sizeof(curr_slave->port)) {
        curr_slave->port = ntohl(curr_slave->port);
        tc_disconnect(&connection_device);
    } else {
        set_disconnected_state(curr_slave) ;
    }
}

void Trick::MonteCarlo::read_machine_name(Trick::MonteSlave *curr_slave) {
    int num_bytes;
    char slave_name[HOST_NAME_MAX];

    tc_read(&connection_device, (char *)&num_bytes, (int)sizeof(num_bytes));
    num_bytes = ntohl(num_bytes) ;
    if (tc_read(&connection_device, slave_name, num_bytes) == num_bytes ) { 
        curr_slave->state = MonteSlave::READY;
        slave_name[num_bytes] = '\0';
        curr_slave->machine_name = std::string(slave_name);
    } else {
        set_disconnected_state(curr_slave) ;
    }
}

void Trick::MonteCarlo::set_disconnected_state(Trick::MonteSlave *curr_slave) {
    curr_slave->state = Trick::MonteSlave::DISCONNECTED;
    if (verbosity >= ERROR) {
        message_publish(MSG_ERROR, "Monte [Master] Lost connection to %s:%d during initialization.\n",
                        curr_slave->machine_name.c_str(), curr_slave->id) ;
    }
}
