
# Trick Master/Slave Synchronization Design

Master/Slave synchronization is an optional class when running simulations.

Master/Slave synchronization synchronizes a master simulation to one or more slave
simulations.  While synchronized, the master controls the overall simulation
mode as well as the current simulation time for all simulations.  The set of
simulations may change mode if one of the simulations in the set freezes or exits
unexpectedly.

Master/Slave syncrhonization uses the Trick::MSConnect class for communications.
The MSConnect class is an abstract base class that defines functions for
connecting the simulations together and passing the simulation mode and
simulation time between simulations.  The Trick::MSSocket class derives from the
Trick::MSConnect class, and uses sockets as the transport medium for master/slave communications.

## Master

The master simulation starts slave simulations during initialization.  At the
end of each software frame during Run mode and at the end of each freeze frame
during Freeze mode the master reads the mode of all connected slave simulations.
The master will determine the mode of the simulations for the next frame
according to the slave modes and the master mode.  The new mode and the
current simulation time is sent to the slaves.

Master/Slave synchronization is divided into code based on the mode of the
simulation.  The modes are Initialization, Run, Freeze, and Shutdown.

Master/Slave functions are designed to be called from the S_define file.

Listed below are the various modes of operation and the Master
responsibilities within each.

### Initialization -- Master

During Initialization slave connections are allocated and defined.  Default
values to start each slave using ssh are set when a slave connection is
allocated.  At the mimimum a slave must define a remote machine, the path 
to the simulation executive, and a Trick::MSConnect class to use for
communications with the slave.  Users may also define the remote shell to use, 
a simulation executable name, a remote run directory, and additional user arguments
to pass to the slave.  The allocation and definition of the slave
connections are typically done through Trick's input processor, but may also be
done in default_data or initialization jobs.

At the end of the scheduler initialization phase the master starts the slaves.
For each slave the master creates a slave startup command and executes the 
slave startup command.  This is how the slave startup command is created:

1. If the MSConnect class is not defined, return an error.
1. Begin the slave startup command with the slave remote shell.
1. Add the remote display environment variable to the startup command if
   one was defined.
1. Add a cd command to the slave simulation path directory to the slave starup
   command.
1. Add the S_main executable name to the slave startup command.
1. Add the Run Input File to the slave startup command.
1. Add additional user arguments to the slave startup command.
1. Execute the slave startup command.

The master sends its software frame to each slave.

### Run -- Master

During Run mode the master reads the simulation mode of each slave.  The
master determines the new simulation mode and sends the new mode and the
current simulation time to the slaves.

1. For each slave
    1. Read the simulation command of the slave.
    1. If the master mode command is not ExitCmd take action if the slave
       returns the following:
        1. If the slave command was not readable, returned ErrorCmd, and:
            1. Terminate the sim if sync_error_terminate == true.
            1. Deactivate the slave if sync_error_terminate == false.
        1. If the slave command is ExitCmd:
            1. Terminate the sim if sync_error_terminate == true.
            1. Deactivate the slave if sync_error_terminate == false.
        1. If the slave command is FreezeCmd:
            1. Freeze the master sim

After gathering the simulation command from all of the slaves, send the
command to all of the slaves.

1. For each slave 
    1. Write the simulation time to the slave.
    1. Write the simulation mode command to the slave.

### Freeze -- Master

Upon entering freeze mode the master turns off sync_time_limit because while
in freeze, master/slave communications may not be regular and periodic.

At the end of each freeze frame call Trick::Master::end_of_frame() to perform the same
synchronization as in run mode.

When exiting freeze mode the master sets sync_time_limit back to the user
set value ready for running again.

### Shutdown -- Master

The master tells the slaves that it is shutting down.

1. For each slave
    1. Write the simulation time to the slave.
    1. Write the simulation mode command ExitCmd to the slave.


## Slave

At the end of each software frame during Run mode and at the end of each
freeze frame during Freeze mode slave simulations send the master the current
mode of the slave.  The slaves wait for the new mode and simulation time from
the master.

### Initialization -- Slave

The Trick::Slave class is enabled by calling the MSConnect::process_sim_args()
routine.  If the MSConnect::process_sim_args() finds command line arguments
specifying this simulation is a slave, then the Trick::Slave is enabled.

At the end of the scheduler initialization phase the slave connects to the
master.

1. Call Trick::MSConnect::connect() to connect the slave to the master.
1. Search for a RealtimeSync class.  If one exists, turn the object off.
1. Search for a Master class.  If one exists, turn the object off.
1. Read and set the slave's software frame from the master.

### Run -- Slave

In Run mode the slave writes its next executive command to the master.  The
master will return the overall next simulation mode to the slave.  The slave
will also read the current simulation time from the master.

1. Write the simulation command to the master.
1. Read the simulation time from the master.
1. if the simulation time read was not successful:
    1. Terminate the sim if sync_error_terminate == true.
    1. Disable master/slave communication and command the sim to freeze if sync_error_terminate == false.
1. If the simulation time read was successful set the slave simulation
   time to the time of the master.
1. Read the simulation command from the master.
1. If the simulation command was not successful:
    1. Terminate the sim if sync_error_terminate == true.
    1. Disable master/slave communication and command the sim to freeze if sync_error_terminate == false.
1. If the simulation command read was successful set the slave simulation
   command to the command of the master.

### Freeze -- Slave

Upon entering freeze mode the slave turns off sync_time_limit because while
in freeze, master/slave communications may not be regular and periodic.

At the end of each freeze frame call Trick::Slave::end_of_frame() to perform the same
synchronization as in run mode.

When exiting freeze mode the slave sets sync_time_limit back to the user
set value ready for running again.

### Shutdown -- Slave

If a slave is shutting down, inform the master that it is exiting.
[@anchor d_slave_shutdown_0 d_slave_shutdown_0]

