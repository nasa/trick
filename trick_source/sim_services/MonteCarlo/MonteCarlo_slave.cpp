
#include "trick/MonteCarlo.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/tc_proto.h"

/** @par Detailed Design: */
int Trick::MonteCarlo::execute_as_slave() {

    /** <li> Forever: */
    while (true) {
        if (verbosity >= ALL) {
            message_publish(MSG_INFO, "Monte [%s:%d] Waiting for new run.\n",
                            machine_name.c_str(), slave_id) ;
        }
        /** <ul><li> On a blocking read, wait for a MonteSlave::Command from the master. */
        if (tc_accept(&listen_device, &connection_device) != TC_SUCCESS) {
            if (verbosity >= ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Lost connection to Master. Shutting down.\n",
                                machine_name.c_str(), slave_id) ;
            }
            slave_shutdown();
        }
        int command;
        if (tc_read(&connection_device, (char *)&command, (int)sizeof(command)) != (int)sizeof(command)) {
            if (verbosity >= ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Lost connection to Master while receiving instructions. Shutting down.\n",
                                machine_name.c_str(), slave_id) ;
            }
            slave_shutdown();
        }
        switch (command = ntohl(command)) {
            int return_value;
            case MonteSlave::PROCESS_RUN:
                /**
                 * <ul><li> MonteSlave::PROCESS_RUN: Call #slave_process_run. This will return a non-zero value when run in a
                 * child process to indicate that this function should return so that the sim can complete.
                 */
                return_value = slave_process_run();
                if (return_value != 0) {
                    return return_value;
                }
                break;
            case MonteSlave::SHUTDOWN:
                /** <li> MonteSlave::SHUTDOWN: Call #slave_shutdown. */
                if (verbosity >= INFORMATIONAL) {
                    message_publish(MSG_INFO, "Monte [%s:%d] Shutdown command received from Master. Shutting down.\n",
                                    machine_name.c_str(), slave_id) ;
                }
                slave_shutdown();
                break;
            case MonteSlave::DIE:
                /** <li> MonteSlave::DIE: Call #slave_die. */
                if (verbosity >= INFORMATIONAL) {
                    message_publish(MSG_INFO, "Monte [%s:%d] Killed by Master.\n",
                                    machine_name.c_str(), slave_id) ;
                }
                slave_die();
                break;
            default:
                /** <li> default: Call #slave_shutdown. */
                if (verbosity >= ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Unrecognized command %d received from Master. Shutting down.\n",
                                machine_name.c_str(), slave_id, command) ;
                }
                slave_shutdown();
                break;
        }
    }
}
