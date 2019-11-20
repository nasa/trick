
# Scheduler Design 

The scheduler design is divided based on the mode of the simulation.  The modes are 
Pre-Initialization, Initialization, Run, Freeze, and Shutdown.  

The scheduler routines are called directly by a main program in a simulation.
A typical main program calls the following scheduler routines.

-# Call add_sim_object() for each sim_object to be added to the scheduler.  
-# Call init() to initialize the simulation. 
-# Call loop() to run the cyclic jobs of the simulation.  loop() contains the
   code for the Run and Freeze modes of operation. 
-# Call shutdown() to shutdown the simulation. 

@dot
digraph finite_state_machine {
     rankdir=LR ;
     size="12,5"
     node [shape = doublecircle]; Initialization Shutdown;
     node [shape = circle] ;
     Initialization -> Run [ label = "no errors" ] ;
     Initialization -> Shutdown [ label = "error condition" ];
     Freeze -> Run [ label = "exec_command = RunCmd" ] ;
     Run -> Freeze [ label = "exec_command = FreezeCmd" ] ;
     Freeze -> Shutdown [ label = "exec_command = ExitCmd\nor error condition" ] ;
     Run -> Shutdown [ label = "exec_command = ExitCmd\nor error condition" ] ;
}
@enddot

<center>Figure 1: Simulation Mode Finite State Machine</center>

Listed below are the various modes of operation and scheduler responsibilities within each.

## Pre-Initialization

During Pre-Initialization the simulation tells the Executive scheduler which SimObjects
are to be included in this simulation run.  The auto-generated initialization routine 
in S_source.cpp calls add_sim_object() for each sim_object instantiated in the S_define file.

### Adding Simulation Objects 

In Pre-Initialization the auto-generated initialization routine calls add_sim_object()
for each sim_object instantiated in the S_define file.  Each add_sim_object() will add
its jobs to the scheduing queues in order of SimObject instantiation.

@copydetails Trick::Executive::add_sim_object(Trick::SimObject * in_object , const char * in_name )
Trick::Executive::add_sim_object(Trick::SimObject * in_object , const char * in_name )

### Adding List of Jobs to Scheduling Queue

Each add_jobs_to_queue() will call add_job_to_queue() to add an individual job to the scheduler queue.

@copydetails Trick::Executive::add_jobs_to_queue(Trick::SimObject *, bool)
Trick::Executive::add_jobs_to_queue(Trick::SimObject *, bool)

### Adding Individual Job to Scheduling Queue

Adds an individual job to the scheduler queues.

@copydetails Trick::Executive::add_job_to_queue(Trick::JobData *)
Trick::Executive::add_job_to_queue(Trick::JobData *)

## Initialization Mode

During initialization mode the scheduler readies the simulation for running.  This includes
calling SimObject initialization jobs as well as initializing scheduler timers.  The Executive
provides the routine to call all default data and initialization class jobs as well as provides
jobs to initialize specific parts of the Executive itself.  The initialization jobs include

1. Executive Initialization
1. Initialize default signal handlers
1. Process Simulation arguments
1. Post Initialization signal handler assignment
1. Write the S_job_execution File
1. Write the S_run_summary file which describes the simulation build environment
1. Check if all jobs in all SimObjects are handled by a scheduler.
1. Create scheduler threads

### Executive Initialization 

This routine is typically called by the simulation main program.  This routine initializes the
scheduler itself as well as calls the initialization routines for all the math models.

@copydetails Trick::Executive::init()
Trick::Executive::init()

### Initialize Default Signal Handlers

@copydetails Trick::Executive::init_signal_handlers()
Trick::Executive::init_signal_handlers()

### Process Simulation Arguments

@copydetails Trick::Executive::process_sim_args()
Trick::Executive::process_sim_args()

### Post Initialization Signal Handler Assignment

@copydetails Trick::Executive::post_init_signal_handlers()
Trick::Executive::post_init_signal_handlers()

### Write the S_job_execution File

The S_job_execution file contains all of the jobs currently scheduled in the Executive.  The file
lists the jobs in order in the queues and shows the job name, id, class, phase, cycle time, start time, and
end time.

@copydetails Trick::Executive::write_s_job_execution()
Trick::Executive::write_s_job_execution()

### Write the S_run_summary File

@copydetails Trick::Executive::write_s_run_summary()
Trick::Executive::write_s_run_summary()

## Run Mode

After initialization the simulation enters the run/freeze loop.  A single loop to handle jobs for
both modes are integrated to provide easy entry and exit points from one loop to the other.

During Run mode the executive/scheduler advances simulation time.
The scheduler calls the runtime cyclic jobs when simulation time equals the next call
time of the job.  The scheduler has a software frame time.  Users may specify override
the default software frame of 1 second.  When the simulation time equals the next software
frame time, the scheduler calls end of software frame jobs.

Multiple threads may have been spawned to execute the Run mode jobs.  Threads may be specified
as synchronous or asynchronous.  Synchronous threads are synchronized to the main scheduler
thread at the start of each time step.  Asynchronous threads are further divided into two types
Asynchronous and Asynchronous Must Finish (AMF) threads. Asynchronous threads are never
synchronized to the main scheduler thread once started.  If an asynchronous thread completes
execution of all of its jobs, the jobs are started again at the next time step.  AMF threads
are only synchronized to the main scheduler thread at a user specified thread specific
frequency.

At the beginning of Run mode the number of threads of execution is checked.
If there is only one thread of execution, a simplified run loop where
multi-thread dependency checks are removed is executed.  Otherwise a
multi-threaded aware loop is called.

There are 5 routines supporting Run mode.

1. Loop Entry Point
1. Single Thread Loop
1. Multi Thread Main Thread Loop
1. Multi Thread Child Thread Loop
1. Advance Simulation Time

### Loop Entry Point

The loop entry point determines if the simulation is multi threaded or not.  It calls the corresponding
loop to do the actual work.

@copydetails Trick::Executive::loop()
Trick::Executive::loop()

### Single Thread Loop

Called by loop().  Calls jobs in the main thread.

@copydetails Trick::Executive::loop_single_thread()
Trick::Executive::loop_single_thread()

### Multi Thread Main Thread Loop

Called by loop().  Coordinates scheduled job calls for the main thread and all child threads.

@copydetails Trick::Executive::loop_multi_thread()
Trick::Executive::loop_multi_thread()

### Multi Thread Child Thread Loop

Child threads are signaled by the main thread to execute their jobs.

@copydetails Trick::Threads::child()
Trick::Threads::child()

### Advance Sim Time

The Executive advances simulation time during each time step with a system_advance_sim_time class
job.  This job advances time to the next lowest job call time.

@copydetails Trick::Executive::advance_sim_time()
Trick::Executive::advance_sim_time()

### Freeze Mode

During Freeze mode, simulation elapsed time does not advance.  The scheduler enters a
loop that calls all freeze jobs at a monotonic frequency.  Jobs may be called to start
freeze mode as well as when exiting freeze mode.  All freeze mode jobs are executed within
the main thread.

@copydetails Trick::Executive::freeze_loop()
Trick::Executive::freeze_loop()

## Shutdown Mode

During Shutdown mode the scheduler waits for asynchronous jobs to finish if so directed.
The scheduler then calls shutdown jobs. The scheduler prints the reason for simulation
termination.

@copydetails Trick::Executive::shutdown()
Trick::Executive::shutdown()

## Signal Handling

Active through all modes, the scheduler assigns signal handlers to attempt a graceful
shutdown in case of a signal being thrown.

@copydetails Trick::Executive::signal_handler()
Trick::Executive::signal_handler()

