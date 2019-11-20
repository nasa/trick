Executing the simulation main executable S_main_${TRICK_HOST_CPU}.exe generates a handful of log files that document a simulation run.

The log files are written to the RUN_<name> directory by default. The destination can be redirected by specifying the -O or -OO option for the runtime executive. Two of the log files are described below.

S_job_execution lists the jobs executed by name, the job types, the arguments, and start and stop times.

S_run_summary documents the name and path of the executable and the input file, the build time of the executable, and the Trick version. It also contains the list of environment variables used when the executable was built and the model versions.

log_<group_name>.* log files and log header files. These files are created by the Data Recording capabilities in Trick. They contain cyclic recorded parameters from within the simulation.

send_hs all messages written by send_hs or message_publish will be saved to the send_hs file.

[Continue to Data Products](../data_products/Data-Products)
