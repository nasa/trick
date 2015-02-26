/*
 * $Id: MonteCarlo_initialize_sockets.cpp $
 */

#include <sys/stat.h>
#include <libgen.h>

#include "sim_services/MonteCarlo/include/MonteCarlo.hh"
#include "sim_services/Message/include/message_proto.h"
#include "trick_utils/comm/include/tc_proto.h"

/** @par Detailed Design: */
int Trick::MonteCarlo::initialize_sockets() {

    /** set tc_error to zero for TCDevices to turn off advisory messages. */
    tc_error(&listen_device, 0);
    tc_error(&connection_device, 0);
    tc_error(&data_listen_device, 0);
    tc_error(&data_connection_device, 0);

    /** <ul><li> Initialize the sockets for communication with slaves. */
    int return_value = socket_init(&listen_device);
    if (return_value != TC_SUCCESS) {
        if (verbosity >= ERROR) {
            message_publish(MSG_ERROR, "Monte [Master] Failed to initialize status communication socket.\n") ;
        }
        return return_value;
    }

    return_value = socket_init(&data_listen_device);
    if (return_value != TC_SUCCESS) {
        if (verbosity >= ERROR) {
            message_publish(MSG_ERROR, "Monte [Master] Failed to initialize data communication socket.\n") ;
        }
        return return_value;
    }

    /** <li> If no slaves were specified, add one on localhost. */
    if (slaves.empty()) {
        if (verbosity >= ALL) {
            message_publish(MSG_WARNING, "Monte [Master] No slaves specified. Adding localhost as the sole slave.\n") ;
        }
        add_slave(new MonteSlave());
    }
    return TC_SUCCESS ;
}
