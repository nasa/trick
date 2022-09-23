| [Home](/trick/index) → [Documentation Home](../Documentation-Home) → Software Requirements Specification |
|-------------------------------------------------------------------------------------------------|

# Introduction
The Software Requirements Specification defines the functional, performance, and interface requirements for Trick.

## Adaptation Requirements
01. Trick source code shall use a POSIX compliant API when accessing POSIX threads (pthreads), mutual exclusions (mutexes), signals, interrupts, and clock routines. [Trick-151]

## Applicable and Reference Documents
### Applicable Documents
The following documents are reference documents utilized in the development of this SRS. These documents do not form a part of this SRS and are not controlled by their reference herein.

<center>
    <table>
    <tr><th>Document Number</th><th>Revision/ Release Date</th><th>Document Title</th></tr>
    <tr><td></td><td></td><td></td></tr>
    </table>
</center>

### Reference Documents
The following documents are reference documents utilized in the development of this SRS. These documents do not form a part of this SRS and are not controlled by their reference herein.

<center>
    <table>
    <tr><th>Document Number</th><th>Revision/ Release Date</th><th>Document Title</th></tr>
    <tr><td></td><td></td><td></td></tr>
    </table>
</center>

### Order of Precedence
In the event of a conflict between the text of this specification and an applicable document cited herein, the text of this specification takes precedence.

All specifications, standards, exhibits, drawings or other documents that are invoked as "applicable" in this specification are incorporated as cited.  All documents that are referred to by an applicable document are considered to be for guidance and information only, with the exception of Interface Control Documents (ICD) which will have their applicable documents considered to be incorporated as cited.

## Command Line Arguments Requirements
01. Trick shall save the number of command line arguments.
01. Trick shall save all command line arguments.
01. Trick shall determine and save the current directory of the simulation executable.
01. Trick shall determine and save the current name of the simulation executable.
01. Trick shall determine and save the current directory of the run input file.
01. Trick shall use the run input directory as the default output directory.
01. Trick shall allow the user to override the output directory.
01. Trick shall provide the option to create timestamped subdirectories within the output directory.

## Data Recording Requirements
01. Data Recording shall provide the capability to record one or more variables that a user has specified in a log group.
01. Data Recording shall provide the capability to record multiple log groups.
01. Data Recording shall time tag the recorded data with the simulation time.
01. Data Recording shall provide the capability to record each log group at its own user specified frequency
01. Data Recording shall provide the capability to record each log group only when user specified model data changes.
01. Data Recording shall provide the capability to write data to disk without affecting the real-time performance of the simulation.

## Design and Implementation Constraints
01. Trick shall not preclude that which cannot be precluded unless Trick is precluded from not precluding the unprecludable.

## Environment Requirements
01. Trick shall plant a tree for every successful simulation built.
01. Trick shall be dolphin safe.
01. Trick shall recycle.

## Scheduler Requirements
### Simulation Time
01. Trick shall track simulation elapsed time by an integer count of tics/second
01. Trick shall initialize simulation elapsed time after all initialization class jobs have completed.
01. Trick shall increment simulation elapsed time to the next lowest job call time greater than the current simulation time after all jobs at the current simulation time have completed.

### Modes
01. Trick shall provide an initialization mode.
01. Trick shall provide a run mode.
01. Trick shall provide a freeze mode.
01. Trick shall provide an exit mode.
01. Trick shall provide a mode command to transition to Freeze.
01. Trick shall provide a mode command to transition to Run.
01. Trick shall provide a mode command to transition to Shutdown.

### Jobs
01. Trick shall provide a method for adding simulation objects to the scheduler prior to execution.
01. Trick shall order jobs by job_class, then phase, then sim_object id, then job_id.

### Periodic Jobs
01. Trick shall execute periodic scheduled jobs during simulation run mode (all scheduled types).
01. Trick shall execute periodic freeze jobs during simulation freeze mode (freeze).
01. Trick shall execute periodic scheduled jobs during simulation run mode at the end of a settable software_frame  (end_of_frame).
01. Trick shall assign the initial call time for a periodic job to the current simulation time + job offset.
01. Trick shall reschedule subsequent job call times for a periodic job to the current simulation time + job cycle.

### Discrete Jobs
01. Trick shall execute discrete job execution scheduling on simulation startup (default_data).
01. Trick shall execute discrete job execution scheduling during simulation initialization (initialization).
01. Trick shall execute of discrete job execution scheduling upon entering simulation freeze (freeze_init).
01. Trick shall execute of discrete job execution scheduling upon exiting simulation freeze (unfreeze).
01. Trick shall execute discrete job execution scheduling during simulation termination (shutdown).

### Instrumentation Jobs
01. Trick shall provide a method for inserting a job before each initialization and each scheduled job within a simulation.
01. Trick shall provide a method for inserting a job before a single initialization or scheduled job within a simulation.
01. Trick shall provide a method for inserting a job after each initialization and each scheduled job within a simulation.
01. Trick shall provide a method for inserting a job after a single initialization or scheduled job within a simulation.
01. Trick shall provide a method for removing instrumentation job(s).

### Threads
01. Trick shall support single threaded execution during all modes of operation.
01. Trick shall support multiple threads of execution during run mode.
01. Trick shall support child threads that synchronize execution with each time step of the parent thread (synchronous thread).
01. Trick shall support child threads that does not synchronize execution with the parent thread. Thread jobs are restarted at next available time step from the master thread.  (asynchronous thread)
01. Trick shall support child threads that does synchronizes execution with the parent thread at a specified interval. This interval may be greater than a time step in the parent thread (asynchronous must finish (AMF) thread).
01. Trick shall spawn enough threads to accomodate jobs listed within the S_define file.
01. Trick shall provide inter-thread job dependencies. This ensures pre-requesite jobs are completed before current job execution.
01. Trick shall terminate simulation execution if a child thread exits.

### Signals
01. Trick shall assign signal handlers to attempt a graceful shutdown of the simulation when the following signals occur: SIGINT, SIGTERM, SIGBUS, SIGSEGV, SIGFPE.
01. Trick shall assign signal handlers to handle the SIGCHLD signal.
01. Trick shall attempt to terminate the simulation gracefully when a SIGTERM, SIGBUS, or SIGSEGV signal is caught. Program corruption at this point may be too great to complete this requirement, hence "attempt".
01. Trick shall allow the user to assign the system default signal handlers for the signals SIGBUS, SIGSEGV, or SIGFPE signals.

## Realtime Clock Requirements
01. Trick shall provide a framework to use a real-time clock source.
01. The framework shall provide a base real-time clock class that provides an interface to initialize the clock.
01. The framework shall provide a base real-time clock class that provides an interface to return the current time.
01. The framework shall provide a base real-time clock class that provides an interface to reset the clock.
01. The framework shall provide a base real-time clock class that provides an interface to wait/spin on the clock to reach a requested time.
01. The framework shall provide a base real-time clock class that provides an interface to stop the clock.
01. Trick shall provide a real-time clock based on the system gettimeofday clock (GetTimeofDay Clock).

## Realtime Synchronization Requirements
01. Trick shall be capable of synchronizing simulation time to a real-time clock.
01. Trick shall be capable of running non-real-time.
01. Trick shall provide the capability to switch between real-time and non-real-time execution.
01. Trick shall detect when the current simulation job execution frame takes longer than the real-time frame (overrun).
01. Trick shall provide the option to respond to simulation overruns by continuing to next frame of execution.
01. Trick shall provide the option to respond to simulation overruns by freezing.
01. Trick shall provide the option to respond to simulation overruns by terminating.
01. The overrun criteria to cause a response shall be either a single large overrun of a user specified size (in seconds), or a user specified number of overruns is detected.
01. Trick shall detect when the current simulation job execution frame is equal to or shorter than the real-time frame (underrun).
01. Trick shall wait for real-time to catch up to the simulation time before starting the next frame of execution during an underrun.
01. Trick shall provide the option to release the processor (sleep) during an underrun.

## Safety Requirements
There are no safety requirements for this software.  Any simulations that use trick functions to control hazards are responsible for maintaining their safety requirements.

## Scheduled Job Queue Requirements.
The ScheduledJobQueue is a helper object to the Scheduler. This object keeps an ordered queue of the jobs for a particular job class. The Scheduler adds and removes jobs from the queue. The Scheduler asks for the top job of the queue, or the next job that has an execution time that matches the current simulation time. Finally, the Scheduler asks the ScheduledJobQueue to insert an instrumentation job where the job is attached before or after a specific job in the queue, or attached before or after all jobs in the simulation.

### Jobs
01. Trick shall store an ordered queue of jobs of the same scheduling class.  The order shall be based on job class, phase number, sim_object order, and finally order of jobs within the simulation object.
01. Trick shall provide a method to add simulation objects to the queue.
01. Trick shall provide a method to add simulation objects to the queue that ignores the sim_object and job order.
01. Trick shall provide a method to clear the job queue.
01. Trick shall provide a method to get the next job on the list.
01. Trick shall provide a method to search for the next job on the list whose next job call time matches the current simulation time.
01. Trick shall provide a method to reset the search index to the top of the list.

### Job Call Time
01. Trick shall track the lowest next job call time greater than the current call time.
01. Trick shall provide a method to retrieve the next job call time.

### Instrumentation Jobs
01. Trick shall provide a method to insert a job before each job in the queue.
01. Trick shall provide a method to insert a job before a single job in the queue.
01. Trick shall provide a method to insert a job after each job in the queue.
01. Trick shall provide a method to insert a job after a single job in the queue.

## Sleep Timer Requirements
01. Trick shall provide a framework to use a sleep timer.
01. The framework shall provide a base timer class that provides an interface to initialize the timer.
01. The framework shall provide a base timer class that provides an interface to start the timer.
01. The framework shall provide a base timer class that provides an interface to reset the timer.
01. The framework shall provide a base timer class that provides an interface to wait on the timer to expire.
01. The framework shall provide a base timer class that provides an interface to stop the timer.
01. The framework shall provide a base timer class that provides an interface to shutdown the timer.
01. Trick shall provide a sleep timer based on the system itimer (ITimer timer).

## Traceability to Parent Requirements
01. There are no formal parent requirements that flow into this SRS.
