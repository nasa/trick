
#include "trick/MonteCarlo.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/tc_proto.h"

/**
 * @par Detailed Design:
 * This function performs a non-blocking accept on the listening socket.
 */
void Trick::MonteCarlo::receive_results() {
    fd_set rfds;
    struct timeval select_time;
    struct timeval timeout_time = {5,0};
    int max_socket;
    Trick::MonteSlave *curr_slave = NULL;
    unsigned int curr_slave_id;

    FD_ZERO(&rfds);
    select_time = timeout_time;

    // add data listen device
    FD_SET(data_listen_device.socket, &rfds);
    max_socket = data_listen_device.socket;

    // add status listen device
    FD_SET(listen_device.socket, &rfds);
    if (max_socket < listen_device.socket) {
        max_socket = listen_device.socket;
    }

    select(max_socket + 1, &rfds, NULL, NULL, &select_time);

    if (FD_ISSET(listen_device.socket, &rfds)) {
        receive_slave_results() ;
    }  // end of FD_ISSET(&listen_device.socket)

    /** <li> Receive results from child */
    while (tc_listen(&data_listen_device)) {
        tc_accept(&data_listen_device, &data_connection_device);
        /** <ul><li> Read the id of incoming curr_slave. */
        tc_read(&data_connection_device, (char*)&curr_slave_id, (int)sizeof(curr_slave_id));
        curr_slave_id = ntohl(curr_slave_id);
        curr_slave = get_slave(curr_slave_id);
        tc_read(&data_connection_device, (char*)&current_run, (int)sizeof(current_run));
        current_run = ntohl(current_run);

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
        /** <li> else run optimization jobs */
        } else {
            curr_slave_id = curr_slave->id;
            run_queue(&master_post_queue, "in master_post queue") ;
            curr_slave_id = 0;
        }
        tc_disconnect(&data_connection_device);
    }
}
