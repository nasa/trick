/*
    PURPOSE: (Monte Carlo C interface.)
*/

#ifndef MONTECARLO_C_INTF_H
#define MONTECARLO_C_INTF_H

#include "trick/tc_proto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @relates Trick::MonteCarlo
 * @copydoc MonteCarlo::set_enabled
 */
void mc_set_enabled(int enabled);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_enabled
 */
int mc_get_enabled();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_dry_run
 */
void mc_set_dry_run(int dry_run);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_dry_run
 */
int mc_get_dry_run();

/**
 * @relates Trick::MonteCarlo
 * @copydoc is_slave
 */
int mc_is_slave();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_localhost_as_remote
 */
void mc_set_localhost_as_remote(int localhost_as_remote);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_localhost_as_remote
 */
int mc_get_localhost_as_remote();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_custom_slave_dispatch
 */
void mc_set_custom_slave_dispatch(int custom_slave_dispatch);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_custom_slave_dispatch
 */
int mc_get_custom_slave_dispatch();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_timeout
 */
void mc_set_timeout(double timeout);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_timeout
 */
double mc_get_timeout();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_max_tries
 */
void mc_set_max_tries(unsigned int max_tries);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_max_tries
 */
unsigned int mc_get_max_tries();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_user_cmd_string
 */
void mc_set_user_cmd_string(const char *user_cmd_string);

/**
 * @relates Trick::MonteCarlo
 * set #Trick::MonteCarlo::slave_sim_options
 */
void mc_set_slave_sim_options(const char *slave_sim_options);

/**
 * @relates Trick::MonteCarlo
 * get #Trick::MonteCarlo::slave_sim_options
 */
const char *mc_get_slave_sim_options();

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_user_cmd_string
 */
const char *mc_get_user_cmd_string();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_custom_pre_text
 */
void mc_set_custom_pre_text(const char *custom_pre_text);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_custom_pre_text
 */
const char *mc_get_custom_pre_text();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_custom_post_text
 */
void mc_set_custom_post_text(const char *custom_post_text);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_custom_post_text
 */
const char *mc_get_custom_post_text();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_verbosity
 */
void mc_set_verbosity(int verbosity);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_verbosity
 */
int mc_get_verbosity();

/**
 * @relates Trick::MonteCarlo
 * @copydoc set_num_runs
 */
void mc_set_num_runs(unsigned int num_runs);

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_num_runs
 */
unsigned int mc_get_num_runs();

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_num_results
 */
unsigned int mc_get_num_results();

/**
 * @relates Trick::MonteCarlo
 * @copydoc get_slave_id
 */
unsigned int mc_get_slave_id();

/**
 * @relates Trick::MonteCarlo
 * @copydoc add_range
 */
void mc_add_range(unsigned int start, unsigned int end);

/**
 * @relates Trick::MonteCarlo
 * @copydoc add_slave(string)
 */
void mc_add_slave(const char *machine_name);

/**
 * @relates Trick::MonteCarlo
 * @copydoc start_slave
 */
void mc_start_slave(unsigned int id);

/**
 * @relates Trick::MonteCarlo
 * @copydoc stop_slave
 */
void mc_stop_slave(unsigned int id);

/**
 * @relates Trick::MonteCarlo
 * Gets the connection_device and tc_write()s the given buffer.
 */
void mc_write(char *buffer, int size);

/**
 * @relates Trick::MonteCarlo
 * Gets the connection_device and tc_read()s the incoming string.
 */
void mc_read(char *buffer, int size);

/**
 * @relates Trick::MonteCarlo
 * Gets the current run being processed.
 */
unsigned int mc_get_current_run() ;

/**
 * @relates Trick::MonteCarlo
 * Sets the current run being processed.
 */
void mc_set_current_run(int num_run) ;

/**
 * @relates Trick::MonteCarlo
 * Sets the port for the listen_device.
 */
void mc_set_listen_device_port(int port_number) ;

/**
 * @relates Trick::MonteCarlo
 * Sets the port for the connection_device.
 */
void mc_set_connection_device_port(int port_number) ;

/**
 * @relates Trick::MonteCarlo
 * Gets the port for the listen_device.
 */
int  mc_get_listen_device_port() ;

/**
 * @relates Trick::MonteCarlo
 * Gets the port for the connection_device.
 */
int  mc_get_connection_device_port() ;

#ifdef __cplusplus
}
#endif
#endif
