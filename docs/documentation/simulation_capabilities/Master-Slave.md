| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Simulation Capabilities](Simulation-Capabilities) → Master/Slave |
|------------------------------------------------------------------|

Master/Slave is a way to do distributed processing in Trick using multiple simulations.
Master/Slave synchronization synchronizes a master simulation to one or more slave
simulations.  The slaves may be run on a different computer or computers, independent of
the computer running the master simulation.

While synchronized, the master controls the overall simulation time for all simulations. Slaves
will use the same real-time software frame as the master and will sync with the master's time at the end of each frame.
The master also controls the overall simulation execution mode for all simulations.  However, a slave that goes into freeze
mode will cause the master to go into freeze mode (which will propagate freeze mode to all slaves). Also,
a slave that loses sync or terminates unexpectedly can affect the master's mode (see sync_error_terminate below).

Trick provides a connection class for synchronization that uses sockets (MSSocket derived from MSConnect) for master/slave 
connection and communication. The user may also provide their own way of synchronization if desired, in which
case you must provide your own class derived from MSConnect.

Master/Slave synchronization is an optional class and need not be specified in 
the S_define file when building a normal (non-Master/Slave) simulation.

## User accessible routines

The following routine is used to create a socket connection for either Master or Slave:

```
Trick::MSSocket::MSSocket() ;
```

In order for Master/Slave synchronization to take place, it must be enabled:

```
int Trick::Master::enable() ;
int Trick::Master::disable() ;
```

The following are routines and attributes for configuring the Master's interface to each Slave:

```
Trick::SlaveInfo::SlaveInfo() ;
int Trick::SlaveInfo::set_connection_type(Trick::MSConnect * in_connection) ;
Trick::SlaveInfo * Trick::Master::add_slave(Trick::SlaveInfo * new_slave) ;
Trick::SlaveInfo::machine_name
Trick::SlaveInfo::machine_display
Trick::SlaveInfo::other_args
Trick::SlaveInfo::remote_shell
Trick::SlaveInfo::remote_shell_args
Trick::SlaveInfo::run_input_file
Trick::SlaveInfo::S_main_name
Trick::SlaveInfo::sim_path
Trick::SlaveInfo::sync_error_terminate
Trick::SlaveInfo::sync_wait_limit
Trick::SlaveInfo::user_remote_shell
```

The following are routines and attributes for configuring a Slave's interface to the Master:

```
int Trick::Slave::set_connection_type(Trick::MSConnect * in_connection) ;
Trick::Slave::sync_error_terminate
```

## Input File Setup

The way to set up Master/Slave synchronization is to use the above routines and attributes
in a Python input file. Here is an example of how to set up a Master and a Slave in their respective input files.

### Master Input File Example

```
new_connection = trick.MSSocket() 
new_slave = trick.SlaveInfo()
new_slave.set_connection_type(new_connection)
new_slave.sim_path = "/users/bob/trick_sims/SIM_myslave"
new_slave.run_input_file = "RUN_test/slave.py"
new_slave.sync_error_terminate = 1
new_slave.sync_wait_limit = 0.2 
master_slave.master.add_slave(new_slave) 
master_slave.master.enable()
```

### Slave Input File Example

```
new_connection = trick.MSSocket() 
master_slave.slave.set_connection_type(new_connection) 
master_slave.slave.sync_error_terminate = 1
```

### Dumping and Loading a Checkpoint

By default, the Master will command the Slave to dump or load a checkpoint when the Master dumps or loads a checkpoint.
The Master will send its checkpoint file name to the Slave, and the Slave will use that file name for its checkpoint (in
its own RUN_ directory).

The following are attributes for configuring the Master's checkpointing interface to each Slave:

```
Trick::SlaveInfo::chkpnt_dump_auto
Trick::SlaveInfo::chkpnt_load_auto
Trick::SlaveInfo::chkpnt_binary
Trick::SlaveInfo::reconnect_wait_limit
```

If you do not want the Slave to dump/load a checkpoint when the Master does, you can turn off either feature in the
Master input file like so:

```
new_slave.chkpnt_dump_auto = 0
new_slave.chkpnt_load_auto = 0
```

in which case your Slave would have to have its own model code to perform a checkpoint dump/load.

When chkpnt_load_auto=1, the Slave restarting and reconnecting should occur within a second or two. If chkpnt_load_auto=0, the user has
to restart the slave himself (and may even be typing in the checkpoint executable on the command line), so reconnect_wait_limit should be
set accordingly.

[Continue to Data Recording](Data-Record)
