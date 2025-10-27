
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "trick/MonteCarlo.hh"
#include "trick/command_line_protos.h"
#include "trick/input_processor_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/tc_proto.h"

/** @par Detailed Design: */
int Trick::MonteCarlo::slave_process_run() {
    int size;
    /** <ul><li> Read the length of the incoming message. */
    if (tc_read(&connection_device, (char *)&size, (int)sizeof(size)) != (int)sizeof(size) || (size = ntohl(size)) < 0) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Lost connection to Master while receiving new run.\nShutting down.\n",
                            machine_name.c_str(), slave_id) ;
        }
        slave_shutdown();
    }
    char *input = new char[size + 1];
    /** <li> Read the incoming message. */
    if (tc_read(&connection_device, input, size) != size) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Lost connection to Master while receiving new run.\nShutting down.\n",
                            machine_name.c_str(), slave_id) ;
        }
        slave_shutdown();
    }
    tc_disconnect(&connection_device);

    // Prepare data and filesystem artifacts in the parent BEFORE fork to avoid
    // doing non-async-signal-safe work in the child process.
    input[size] = '\0';

    // Determine the output directory for each run from the input
    // that includes trick.set_output_dir(".../RUN_xxxxx").
    // See Trick::MonteCarlo::dispatch_run_to_slave for how trick.set_output_dir is used.
    std::string output_dir = command_line_args_get_output_dir();
    std::string run_output_dir = output_dir;  // Default to shared MONTE_* directory
    {
        std::string input_str(input);
        const std::string marker = "trick.set_output_dir(\"";
        size_t start_pos = input_str.find(marker);
        if (start_pos != std::string::npos) {
            start_pos += marker.length();
            size_t end_pos = input_str.find('"', start_pos);
            if (end_pos != std::string::npos) {
                run_output_dir = input_str.substr(start_pos, end_pos - start_pos);
            }
        }
    }

    // Ensure the target run directory exists (create it if does not exist).
    if (access(run_output_dir.c_str(), F_OK) != 0) {
        if (mkdir(run_output_dir.c_str(), 0775) == -1) {
            if (verbosity >= MC_ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Unable to create run dir %s.\nShutting down.\n",
                                machine_name.c_str(), slave_id, run_output_dir.c_str());
            }
            slave_shutdown();
        }
    }

    // Write a monte_input file into the run directory for each run.
    std::stringstream ss_monte_input;
    ss_monte_input << run_output_dir << "/monte_input";
    // Check the path length before creating the file. If too long, aborts.
    if (run_output_dir.size() + 1 + strlen("monte_input") >= PATH_MAX) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Target path too long for monte_input: %s/monte_input.\nShutting down.\n",
                            machine_name.c_str(), slave_id, run_output_dir.c_str());
        }
        slave_shutdown();
    }
    {
        FILE *fp = fopen(ss_monte_input.str().c_str(), "w");
        if (fp == nullptr) {
            if (verbosity >= MC_ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Unable to open monte_input for write.\nShutting down.\n",
                                machine_name.c_str(), slave_id);
            }
            slave_shutdown();
        }

        fprintf(fp,
          "# This run can be executed in stand alone (non-Monte Carlo) mode by running\n"
          "# the S_main executable with this file specified as the input file.\n\n");
        fprintf(fp, "if (sys.version_info > (3, 0)):\n");
        fprintf(fp, "    exec(open(\"%s\").read())\n", command_line_args_get_input_file());
        fprintf(fp, "else:\n");
        fprintf(fp, "    execfile(\"%s\")\n\n", command_line_args_get_input_file());
        fprintf(fp, "trick.mc_set_enabled(0)\n");
        fprintf(fp, "%s" , input);
        fclose(fp);
    }

    // Precompute stdout/stderr paths in fixed buffers for child to use with open/dup2 (for each run directory).
    char stdout_path[PATH_MAX];
    char stderr_path[PATH_MAX];
    int nout = snprintf(stdout_path, sizeof(stdout_path), "%s/%s", run_output_dir.c_str(), "stdout");
    if (nout < 0 || nout >= sizeof(stdout_path)) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Target path too long for stdout: %s/%s.\nShutting down.\n",
                            machine_name.c_str(), slave_id, run_output_dir.c_str(), "stdout");
        }
        slave_shutdown();
    }
    int nerr = snprintf(stderr_path, sizeof(stderr_path), "%s/%s", run_output_dir.c_str(), "stderr");
    if (nerr < 0 || nerr >= sizeof(stderr_path)) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Target path too long for stderr: %s/%s.\nShutting down.\n",
                            machine_name.c_str(), slave_id, run_output_dir.c_str(), "stderr");
        }
        slave_shutdown();
    }

    /**
     * <li> fork() a child process to execute the simulation.
     * This allows the slave to monitor the child and continue running
     * if it crashes or times out.
     * NOTE: fork is being called from a multi-threaded process as a trick sim
     * is a multi-threaded program. Per POSIX spec, fork in a multi-threaded process, 
     * only call async-signal-safe functions until exec a new program.
     * Most heavy work and file creation have been moved to the parent pre-fork.
     * In the child below only use open/dup2, sigaction, and alarm that are deemed 
     * as async-signal-safe before entering the run.
     */
    pid_t pid = fork();
    if (pid == -1) {
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Unable to fork new process for run.\nShutting down.\n",
                            machine_name.c_str(), slave_id) ;
        }
        slave_shutdown();
    /** <li>Parent process: */
    } else if (pid != 0) {
        int return_value = 0 ;
        /** <li> Wait for the child to finish. */
        if (waitpid(pid, &return_value, 0) == -1) {
            /* (Alex) On the Mac this check gives a lot of false positives.  I've commented out the code for now. */
            /*
            if (verbosity >= MC_ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Error while waiting for run to finish.\nShutting down.\n",
                                machine_name.c_str(), slave_id) ;
            }
            slave_shutdown();
            */
        }

        if (WIFEXITED(return_value)) {
            // A successful sim sends its exit status to the master itself in
            // its shutdown job. Users can subvert this by calling exit, in
            // which case the master will eventually deem this run to have
            // timed out. But who would do that?!
            return 0;
        }

        int signal = WTERMSIG(return_value);
        /** <li> Extract the exit status of the child. */
        MonteRun::ExitStatus exit_status = signal == SIGALRM ? MonteRun::MC_RUN_TIMED_OUT : MonteRun::MC_RUN_DUMPED_CORE;
        if (verbosity >= MC_ERROR) {
            message_publish(MSG_ERROR, "Monte [%s:%d] Run killed by signal %d: %s\n",
                            machine_name.c_str(), slave_id, signal, strsignal(signal)) ;
        }
        connection_device.port = master_port;
        if (tc_connect(&connection_device) != TC_SUCCESS) {
            if (verbosity >= MC_ERROR) {
                message_publish(MSG_ERROR, "Monte [%s:%d] Lost connection to Master before results could be returned.\nShutting down.\n",
                                machine_name.c_str(), slave_id) ;
            }
            slave_shutdown();
        }
        if (verbosity >= MC_ALL) {
            message_publish(MSG_INFO, "Monte [%s:%d] Sending run exit status to master %d.\n",
                            machine_name.c_str(), slave_id, exit_status) ;
        }
        /** <li> Write the slaves id to the master. </ul> */
        int id = htonl(slave_id);
        tc_write(&connection_device, (char *)&id, (int)sizeof(id));
        /** <li> Write the child's exit status to the master. </ul> */
        return_value = htonl(exit_status);
        tc_write(&connection_device, (char *)&return_value, (int)sizeof(return_value));
        tc_disconnect(&connection_device);
        return 0;
    /** <li> Child process: */
    } else {
        // Child: redirect stdout/stderr first using async-signal-safe syscalls
        // so that any output from subsequent steps is captured to files.
        int fd_out = open(stdout_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out >= 0) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        int fd_err = open(stderr_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_err >= 0) {
            dup2(fd_err, STDERR_FILENO);
            close(fd_err);
        }

        // input already null-terminated above (pre-fork)
        if ( ip_parse(input) != 0 ) {
            exit(MonteRun::MC_PROBLEM_PARSING_INPUT);
        }
        delete [] input;

        /** <li> Run the pre run jobs. */
        run_queue(&slave_pre_queue, "in slave_pre queue") ;

        /** <li> Set a timer to interrupt us after the timeout value. */
        struct sigaction default_alarm;
        default_alarm.sa_handler = SIG_DFL;
        sigaction(SIGALRM, &default_alarm, NULL);
        alarm((unsigned int)timeout);

        /**
         * <li> Return a non-zero result so the calling function (#slave)
         * will return, allowing the slave sim to complete.
         */
        return 1;
    }
    return 0;
}
