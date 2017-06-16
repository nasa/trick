/*
     PURPOSE: (Monte Carlo simulation.)
*/

#ifndef MONTECARLO_HH
#define MONTECARLO_HH

#include <deque>
#include <vector>
#include <climits>

#include "trick/MonteVar.hh"
#include "trick/Executive.hh"
#include "trick/RemoteShell.hh"
#include "trick/tc.h"

#ifdef SWIG
// This instructs SWIG to use dynamic_cast and return one of the derived type for the get_variable function.
%factory(Trick::MonteVar * Trick::MonteCarlo::get_variable, Trick::MonteVarCalculated, Trick::MonteVarFile, Trick::MonteVarFixed, Trick::MonteVarRandom) ;
#endif

namespace Trick {

    /**
     * Represents a particular iteration in a Monte Carlo simulation. In addition to some bookkeeping information, a run
     * contains the variable values specific to this iteration.
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteRun {

        public:
        /** Details the manner in which this run exited. */
        enum ExitStatus {
            INCOMPLETE, /**< not completed */
            COMPLETE,   /**< completed with no errors */
            CORED,      /**< core dumped */
            TIMEDOUT,   /**< timed out */
            NO_PERM,    /**< could not write output files */
            BAD_INPUT,  /**< problem parseing monte carlo input */
            UNKNOWN     /**< unrecognized return code */
        };

        /** Unique identifier sequentially assigned, starting at zero, by the master. */
        unsigned int id;           /**< \n trick_units(--) */

        /** Number of times this run has been dispatched. */
        unsigned int num_tries;    /**< \n trick_units(--) */

        /** Time at which this run began. */
        double start_time;         /**< \n trick_units(--) */

        /** Time at which this run ended. */
        double end_time;           /**< \n trick_units(--) */

        /** Variable values specific to this Monte Carlo iteration. */
        std::vector <std::string> variables; /**< \n trick_units(--) */

        /** Manner in which this run exited. */
        ExitStatus exit_status;    /**< \n trick_units(--) */

        /**
         * Constructs a MonteRun with the specified id.
         *
         * @param id unique indentifier
         */
        MonteRun(unsigned int in_id) :
            id(in_id),
            num_tries(0),
            start_time(0),
            end_time(0),
            exit_status(INCOMPLETE) {}

    };

    /**
     * Represents a slave in a Monte Carlo simulation.
     *
     * @see @ref MonteCarloSlaves "Slaves"
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteSlave {

        public:
        /** Operational state. */
        enum State {
            UNINITIALIZED,         /**< newly created */
            INITIALIZING,          /**< starting up */
            READY,                 /**< awaiting new run */
            RUNNING,               /**< processing a run */
            STOPPING,              /**< stopping after current run */
            STOPPED,               /**< not accepting new runs */
            FINISHED,              /**< completed all runs */
            UNRESPONSIVE_RUNNING,  /**< timed out and in a running state */
            UNRESPONSIVE_STOPPING, /**< timed out and in a stopping state */
            DISCONNECTED           /**< lost connection */
        };

        /** Master-to-slave commands. */
        enum Command {
            PROCESS_RUN, /**< process a new run */
            SHUTDOWN,    /**< kill any executing run, call shutdown jobs, and shutdown cleanly */
            DIE          /**< kill any executing run, do not call shutdown jobs, and exit */
        };

        /** Unique identifier assigned by the master. */
        unsigned int id;                 /**< \n trick_units(--) */

        /** Operational state. */
        State state;                     /**< \n trick_units(--) */

        /** Name of the machine on which this slave is running. */
        std::string machine_name;             /**< \n trick_units(--) */

        /** Port over which this slave is listening for dispatches. */
        unsigned int port;               /**< \n trick_units(--) */

        /** Run most recently dispatched to this slave. */
        MonteRun *current_run;           /**< \n trick_units(--) */

        /** Number of runs dispatched to this slave. */
        unsigned int num_dispatches;     /**< \n trick_units(--) */

        /** Number of results returned by this slave. */
        unsigned int num_results;        /**< \n trick_units(--) */

        /** Total cpu time used. */
        double cpu_time;                 /**< \n trick_units(--) */

        /** Shell command with which to start this slave. */
        Trick::RemoteShell remote_shell; /**< \n trick_units(--) */

        /**
         * User defined shell command with which to start this slave when the #remote_shell is
         * Trick::TRICK_USER_REMOTE_SHELL.
         */
        std::string user_remote_shell;        /**< \n trick_units(--) */

        /** Optional arguments to use with the remote shell command. */
        std::string remote_shell_args;        /**< \n trick_units(--) */

        /** Multiplier speed of this slave's machine. */
        double multiplier;               /**< \n trick_units(--) */

        /** Remote program directory pathname. */
        std::string sim_path;                 /**< \n trick_units(--) */

        /** Remote program name. */
        std::string S_main_name;              /**< \n trick_units(--) */

        void set_S_main_name(std::string name);    /**< \n trick_units(--) */

        /**
         * Constructs a MonteSlave with the specified machine name.
         *
         * @param name the slave's machine's name
         */
        MonteSlave(std::string name = "localhost") :
            id(0),
            state(UNINITIALIZED),
            port(0),
            current_run(NULL),
            num_dispatches(0),
            num_results(0),
            cpu_time(0),
            remote_shell(Trick::TRICK_SSH),
            multiplier(1) {
            if (name.empty()) {
                machine_name = "localhost";
            }
            else {
                machine_name = name;
            }
        }

    };

    /**
     * Specifies a range of valid run numbers.
     *
     * @see MonteRun::id
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteRange {

        protected:
        /** Starting run number. */
        unsigned int start;         /**< \n trick_units(--) */

        /** Ending run number. */
        unsigned int end;           /**< \n trick_units(--) */

        public:
        /**
         * Constructs a MonteRange with the specified inclusive end points. Specifying an end value that is less than the
         * start value results in a range which includes only the start value.
         *
         * @param start starting run number
         * @param end ending run number
         */
        MonteRange(unsigned int in_start, unsigned int in_end) :
            start(in_start),
            end(in_end) {
            if (end < start) {
                this->end = start;
            }
        }

        /** Gets #start. */
        unsigned int get_start() {
            return start;
        }

        /** Gets #end. */
        unsigned int get_end() {
            return end;
        }

    };

    /**
     * Represents a Monte Carlo simulation.
     *
     * @see @ref MonteCarloPage "Monte Carlo"
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteCarlo : public Trick::Scheduler {

        public:
        /** Verbosity of message reporting. */
        enum Verbosity {
            NONE,          /**< report no messages */
            ERROR,         /**< report error messages */
            INFORMATIONAL, /**< report error and informational messages, no warning messages */
            ALL            /**< report all messages (error, informational & warning) */
        };

        /** Options to be passed to the slave sim. */
        std::string slave_sim_options;

        private:
        int run_queue(Trick::ScheduledJobQueue* queue, std::string in_string) ;

        int open_file(std::string file_name, FILE** file_ptr) ;

        void write_to_run_files(std::string file_name) ;

        int initialize_sockets() ;

        int construct_run_directory() ;

        void shutdown_slaves() ;

        void print_statistics(FILE** fp) ;

        void dryrun() ;

        void initialize_slave(Trick::MonteSlave* slave_to_init) ;

        void default_slave_dispatch_pre_text(Trick::MonteSlave*, std::string &buffer) ;

        protected:
        /** Indicates whether or not this is a Monte Carlo simulation. */
        bool enabled;                                    /**< \n trick_units(--) */

        /**
         * Indicates whether or not this is a dry run. A dry run executes pre run jobs only. Post run jobs and the runs
         * themselves are not executed.
         */
        bool dry_run;                                   /**< \n trick_units(--) */

        /**
         * Indicates whether or not the localhost should be treated as a remote machine. This determines if slaves running
         * locally use remote shells.
         */
        bool localhost_as_remote;                       /**< \n trick_units(--) */

        /**
         * Indicates how much automation should be employed in forming the commands used to dispatch slaves.
         * - A value of <code>true</code> indicates that Trick should form only the core of the command, which consists of
         * S_main executable with the proper slave-specific arguments. #custom_pre_text and #custom_post_text will then be
         * prepended and appended to the core, respectively, before executing the command. #remote_shell_args,
         * #user_cmd_string, MonteSlave::remote_shell, and MonteSlave::machine_name are ignored.
         * - A value of <code>false</code> indicates that Trick should use the shell specified by MonteSlave::remote_shell
         * with the #remote_shell_args, followed by the slave's machine name, #user_cmd_string, navigation to the correct
         * directory, and finally the core command described above. #custom_pre_text and #custom_post_text are ignored.
         */
        bool custom_slave_dispatch;                     /**< \n trick_units(--) */

        /** Maximum time to wait for a run to complete. Defaults to 120 seconds. */
        double timeout;                                 /**< \n trick_units(s) */

        /** Maximum number of times that a run may be dispatched. Defaults to two. Specify zero for no limit. */
        unsigned int max_tries;                         /**< \n trick_units(--) */

        /** Options to be passed to the remote shell when spawning new slaves. */
        std::string user_cmd_string;                         /**< \n trick_units(--) */

        /** Text to be prepended to the core slave dispatch command as described in #custom_slave_dispatch. */
        std::string custom_pre_text;                         /**< \n trick_units(--) */

        /** Text to be appended to the core slave dispatch command as described in #custom_slave_dispatch. */
        std::string custom_post_text;                        /**< \n trick_units(--) */

        /** Highest level of messages to report. */
        Verbosity verbosity;                            /**< \n trick_units(--) */

        /** Device over which connections are accepted. */
        TCDevice listen_device;                         /**< \n trick_units(--) */

        /** Device over which data is sent and received. */
        TCDevice connection_device;                     /**< \n trick_units(--) */

        /** Runs to be dispatched. */
        std::deque <Trick::MonteRun *> runs;                 /**< \n trick_io(**) trick_units(--) */

        /** Failed runs. */
        std::deque <Trick::MonteRun *> failed_runs;          /**< \n trick_io(**) trick_units(--) */

        /** Valid ranges. */
        std::vector <Trick::MonteRange *> run_ranges;        /**< \n trick_io(**) trick_units(--) */

        /** Variables. */
        std::vector <Trick::MonteVar *> variables;           /**< \n trick_io(**) trick_units(--) */

        /** Slaves. */
        std::vector <Trick::MonteSlave *> slaves;            /**< \n trick_io(**) trick_units(--) */

        /** Number of slaves. Exists for Variable Server access. */
        int num_slaves;                                 /**< \n trick_units(--) */

        /** List of slave pointers. Exists for Variable Server access. */
        Trick::MonteSlave **slaves_head;                /**< \n trick_units(--) */

        /** Current run dispatched. */
        unsigned int current_run;                       /**< \n trick_units(--) */

        /** User-specified number of runs. */
        unsigned int num_runs;                          /**< \n trick_units(--) */

        /** Number of runs in range. */
        unsigned int actual_num_runs;                   /**< \n trick_units(--) */

        /** Number of results. */
        unsigned int num_results;                       /**< \n trick_units(--) */

        /** Time simulation began. */
        double start_time;                              /**< \n trick_units(--) */

        /** Time simulation ended. */
        double end_time;                                /**< \n trick_units(--) */

        /** Port on which the master is listening. This value is unspecified for the master. */
        unsigned int master_port;                       /**< \n trick_units(--) */

        /** Unique identifier. This value is zero for the master. */
        unsigned int slave_id;                          /**< \n trick_units(--) */

        /** Name of the machine on which this simulation is running. */
        std::string machine_name;                            /**< \n trick_units(--) */

        /** Run data file. */
        FILE *run_data_file;                            /**< \n trick_io(**) */

        /** Run header file. */
        FILE *run_header_file;                          /**< \n trick_io(**) */

        /** Run directory. */
        std::string run_directory;                      /**, \n trick_units(--) */

        Trick::JobData * curr_job ;                     /**< trick_io(**) */

        /** Return code to be returned by Executive:init(), Executive::loop(), and Executive::shutdown() \n */
        int except_return ;                       /**< trick_io(**) */

        /** File name of exceptions caught in init() and loop()\n */
        std::string except_file ;                      /**< trick_io(**) */

        /** Error message of exceptions caught in init() and loop()\n */
        std::string except_message ;                   /**< trick_io(**) */

        /** Jobs to be run by the master during initialization. */
        Trick::ScheduledJobQueue master_init_queue;     /**< \n trick_units(--) */

        /** Jobs to be run by the master before each run. */
        Trick::ScheduledJobQueue master_pre_queue;      /**< \n trick_units(--) */

        /** Jobs to the run by the master after each run. */
        Trick::ScheduledJobQueue master_post_queue;     /**< \n trick_units(--) */

        /** Jobs to be run by the master during shutdown. */
        Trick::ScheduledJobQueue master_shutdown_queue; /**< \n trick_units(--) */

        /** Jobs to be run by the slave during initialization. */
        Trick::ScheduledJobQueue slave_init_queue;      /**< \n trick_units(--) */

        /** Jobs to be run by the slave before each run. */
        Trick::ScheduledJobQueue slave_pre_queue;       /**< \n trick_units(--) */

        /** Jobs to the run by the slave after each run. */
        Trick::ScheduledJobQueue slave_post_queue;      /**< \n trick_units(--) */

        /** Jobs to be run by the slave during shutdown. */
        Trick::ScheduledJobQueue slave_shutdown_queue;  /**< \n trick_units(--) */

        public:
        /** Constructs a new MonteCarlo. */
        MonteCarlo();

        ~MonteCarlo();

        /**
         * S_define level job.  Sends sims through master/slave logic if monte carlo is enabled.
         *
         * @return 0 on success
         */
        int execute_monte();

        /**
         * Sets #enabled.
         *
         * @see @ref MonteCarloEnabling "Enabling Monte Carlo"
         */
        void set_enabled(bool enabled);

        /**
         * Gets #enabled.
         *
         * @see @ref MonteCarloEnabling "Enabling Monte Carlo"
         */
        bool get_enabled();

        /**
         * Sets #dry_run.
         *
         * @see @ref MonteCarloDryRun "Dry Run"
         */
        void set_dry_run(bool dry_run);

        /**
         * Gets #dry_run.
         *
         * @see @ref MonteCarloDryRun "Dry Run"
         */
        bool get_dry_run();

        /**
         * Returns true if executive is running as the slave,
         * based on value of slave_id (which is > 0 for slave).
         */
        bool is_slave();

        /**
         * Returns true if executive is running as the master,
         * based on the value of slave_id (which is 0 for master).
         */
        bool is_master();

        /**
         * Sets #localhost_as_remote.
         */
        void set_localhost_as_remote(bool localhost_as_remote);

        /**
         * Gets #localhost_as_remote.
         */
        bool get_localhost_as_remote();

        /**
         * Sets #custom_slave_dispatch.
         */
        void set_custom_slave_dispatch(bool custom_slave_dispatch);

        /**
         * Gets #custom_slave_dispatch.
         */
        bool get_custom_slave_dispatch();

        /**
         * Sets #timeout.
         */
        void set_timeout(double timeout);

        /**
         * Gets #timeout.
         */
        double get_timeout();

        /**
         * Sets #max_tries.
         */
        void set_max_tries(unsigned int max_tries);

        /**
         * Gets #max_tries.
         */
        unsigned int get_max_tries();

        /**
         * Sets #user_cmd_string.
         */
        void set_user_cmd_string(std::string user_cmd_string);

        /**
         * Gets #user_cmd_string.
         */
        std::string get_user_cmd_string();

        /**
         * Sets #custom_pre_text.
         */
        void set_custom_pre_text(std::string custom_pre_text);

        /**
         * Gets #custom_pre_text.
         */
        std::string get_custom_pre_text();

        /**
         * Sets #custom_post_text.
         */
        void set_custom_post_text(std::string custom_post_text);

        /**
         * Gets #custom_post_text.
         */
        std::string get_custom_post_text();

        /**
         * Sets #verbosity.
         */
        void set_verbosity(Verbosity verbosity);

        /**
         * Gets #verbosity.
         */
        Verbosity get_verbosity();

        /**
         * Sets #num_runs.
         *
         * @see @ref MonteCarloRuns "Specifying the Number of Runs"
         */
        void set_num_runs(unsigned int num_runs);

        /**
         * Gets #num_runs.
         *
         * @see @ref MonteCarloRuns "Specifying the Number of Runs"
         */
        unsigned int get_num_runs();

        /**
         * Gets #num_results.
         */
        unsigned int get_num_results();

        /**
         * Gets #slave_id.
         */
        unsigned int get_slave_id();

        /**
         * Adds the specified range to the list of valid ranges.
         *
         * @param start the starting run's id
         * @param end the ending run's id
         *
         * @see MonteRun::id
         * @see MonteRange
         * @see @ref MonteCarloRanges "Specifying Valid Ranges"
         */
        void add_range(unsigned int start, unsigned int end = 0);

        /**
         * Determines if the specified run falls within a valid range.
         *
         * @param run the run in question
         *
         * @see @ref MonteCarloRanges "Specifying Valid Ranges"
         */
        bool in_range(Trick::MonteRun *run);

        /**
         * Copies the current vector of valid run ranges into the argument vector.
         *
         * @param ranges the vector into which the ranges will be copied
         *
         * @see @ref MonteCarloRanges "Specifying Valid Ranges"
         */
        void get_ranges(std::vector<MonteRange *> &ranges);

        /**
         * Adds the specified variable.
         *
         * @param variable the variable to add
         *
         * @see @ref MonteCarloVariables "Adding Variables"
         */
        void add_variable(Trick::MonteVar *variable);

        /**
         * Gets the specified variable.
         *
         * @param variable name to get
         */
        Trick::MonteVar * get_variable(std::string variable_name);

        /**
         * Adds a new slave with the specified machine name.
         *
         * @param machine_name the target machine's name
         *
         * @see @ref MonteCarloAddingSlaves "Adding Slaves"
         */
        void add_slave(std::string machine_name);

        /**
         * Adds the specified slave.
         *
         * @param slave the slave to add
         *
         * @see @ref MonteCarloAddingSlaves "Adding Slaves"
         */
        void add_slave(Trick::MonteSlave *slave);

        /**
         * Starts the slave with the specified id if it exists. Starting a slave puts it into a state in which it
         * continuously accepts and processes runs from the master.
         *
         * @param id the id of the slave to start
         *
         * @see MonteSlave::id
         */
        void start_slave(unsigned int id);

        /**
         * Stops the slave with the specified id if it exists. Stopping a slave puts it into a state in which it will not
         * accept new runs from the master.
         *
         * @param id the id of the slave to stop
         *
         * @see MonteSlave::id
         */
        void stop_slave(unsigned int id);

        /**
         * Disables the slave at initialization of the Master. Must be called before master_init is called
         * (i.e. in input file, default_data jobs, or initialization jobs with a phase number = 0)
         *
         * @param name the name of the slave to disable
         * @param disabled true if the slave should be disabled, false if the slave should be used.
         */
        void disable_slave(std::string name, bool disabled);

        /**
         * Processes command line arguments specific to Monte Carlo simulations.
         *
         * @return 0 on success
         */
        int process_sim_args();

        /**
         * Performs a Monte Carlo specific shutdown before calling Executive::shutdown().
         *
         * @return 0 on success
         */
        int shutdown();

        /** Gets #current_run being processed
         *
         * @return the current run number
         */
        unsigned int get_current_run() ;

        /** Sets the #current_run being processed
         *
         * @param run_num the number to set the run
         */
        void set_current_run(int run_num) ;

        /** Allows the user to set the port number for
         * the listen_device
         *
         * @param port_number number for the port
         */
        void set_listen_device_port(int port_number) ;

        /** Allows the user to set the port number for
         * the connection_device
         *
         * @param port_number number for the port
         */
        void set_connection_device_port(int port_number) ;

        /** Allows the user to get the port number for
         * the listen_device
         *
         * @return the port number
         */
        int  get_listen_device_port() ;

        /** Allows the user to get the port number for
         * the connection_device
         *
         * @return the port number
         */
        int  get_connection_device_port() ;

        int write(char* data, int size);
        int read(char* data, int size);

#if 0
        /**
            Overload default implentation of Scheduler::add_sim_object
         */
        virtual int add_sim_object( Trick::SimObject * in_object ) ;
#endif

        protected:
        /**
         * Initializes the master.
         *
         * @return 0 on success
         */
        int master_init();

        /**
         * Begins Monte Carlo simulation as the master.
         *
         * @return 0 on success
         */
        int master();

        /**
         * Spawns all uninitialized slaves.
         *
         * @see MonteSlave::state
         */
        void spawn_slaves();

        /** Receives from any slaves that are ready to return results. */
        void receive_results();

        void handle_initialization(MonteSlave& slave);
        void handle_run_data(MonteSlave& slave);
        void set_disconnected_state(MonteSlave& slave);

        /**
         * Handles the retrying of the current run of the specified slave with the specified exit status.
         *
         * @param slave the slave processing the run
         * @param exit_status the exit status of the run
         *
         * @see max_tries
         */
        void handle_retry(MonteSlave& slave, MonteRun::ExitStatus exit_status);

        /**
         * Resolves the current run of the specified slave with the specified exit status.
         *
         * @param slave the slave processing the run
         * @param exit_status the exit status of the run
         */
        void resolve_run(MonteSlave& slave, MonteRun::ExitStatus exit_status);

        /** Checks dispatched runs for timeouts. */
        void check_timeouts();

        /**
         * Gets a slave that is ready for a new dispatch.
         *
         * @return a ready slave, or <code>NULL</code> if there is none
         */
        MonteSlave *get_ready_slave();

        /**
         * Gets the slave with the specified id.
         *
         * @param id the slave's id
         *
         * @return the slave, or <code>NULL</code> if no such slave exists
         *
         * @see MonteSlave::id
         */
        MonteSlave *get_slave(unsigned int id);

        /**
         * Gets the index within #slaves for the slave with the specified id.
         *
         * @param id the slave's id
         *
         * @return the slave's index, or -1 if no such slave exists
         *
         * @see MonteSlave::id
         */
        int get_slave_index(unsigned int id);

        /**
         * Gets the next run to be dispatched.
         *
         * @return the next run, or <code>NULL</code> if there is none
         */
        MonteRun *get_next_dispatch();

        /**
         * Prepares the specified run for dispatch.
         *
         * @param run the run to initialize
         */
        int  prepare_run(MonteRun *run);

        /** Removes the specified run, if present, from #runs. */
        void dequeue_run(MonteRun *run);

        /**
         * Dispatches the specified run to the specified slave.
         *
         * @param run the run to dispatch
         * @param slave the target slave
         */
        void dispatch_run_to_slave(MonteRun *run, MonteSlave *slave);

        /** Updates the #num_slaves and #slaves_head to reflect the #slaves. */
        void sync_slaves_head();

        /** Updates #actual_num_runs. */
        void update_actual_num_runs();

        /** Shuts down the master. */
        void master_shutdown();

        /**
         * Initializes the slave.
         *
         * @return 0 on success
         */
        int slave_init();

        /**
         * Begins Monte Carlo simulation as a slave.
         *
         * @return 0 on success
         */
        int execute_as_slave();

        /** Processes an incoming run. */
        int slave_process_run();

        /** Shuts down the slave. */
        void slave_shutdown();

        /** Kills the slave. */
        void slave_die();

        /** Kills the current run. */
        void slave_kill_run();

        int instrument_job_before(Trick::JobData* instrument_job);
        int instrument_job_after(Trick::JobData* instrument_job);
        int instrument_job_remove(std::string in_job);
        int write_s_job_execution(FILE* fp);


        /**
         * Determines if the specified stings are equivalent, ignoring case.
         *
         * @param string1 the first string
         * @param string2 the second string
         *
         * @return the case-insensitive equivalency
         */
        bool equals_ignore_case(std::string string1, std::string string2); // I am appalled by having to write this myself.

    };

};
#endif
