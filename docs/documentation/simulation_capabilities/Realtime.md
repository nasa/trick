
Trick provides a real-time processing capability which is different from many real-time
simulation capabilities. The Trick executive is a "time based" executive and can run in
a non-real-time mode just as easily as a real-time mode. This is because Trick guarantees
job execution orders and allows the developer to design guaranteed interfaces which are
not affected by the execution time required for any one or more jobs. Frame based
scheduling executives typically have problems handling real-time overruns because the
frame pulse is the scheduling cue. The frame-pulse in Trick is a mechanism to monitor
and maintain the real-time status of the simulation, not a scheduling mechanism.

An executive for a real-time simulation must guarantee that the simulated time matches
the real-world time at specified intervals (real-time frame length). If the simulated
execution time is greater than the real-world time, an overrun has occurred and must be
dealt with. The parent executive process does all job scheduling and real-time frame
processing. Real-time frame processing is guaranteed by synchronizing with a clock.
Trick provides a clock based on the operating system gettimeofday system call.  Users
may provide a different clock my extending the Trick::Clock class.

The software time checks are performed at the end of each real-time
frame by getting the time from the Trick::Clock class, and then
comparing the real-world time to the simulated time.

The gettimeofday clock alone cannot guarantee that an overrun will be detected when
the real-time frame has elapsed; e.g.  the software check will never stop an infinite loop.

When the executive has reached the end of frame it burns a CPU (while loop waiting
for the clock to reach the end of the real-time frame) during an under-run.
Trick provides a sleep timer to pause simulation execution and wait for real-time to
catch up. Trick provides an abstract Trick::Timer class and a Trick::ITimer (SIGALARM)
class.  The itimer uses signals to "wake up" the simulation when real-time has caught
up to simulation time.  Users may provide their own wait mechanisms by extending the
Trick::Timer class.

If overruns occur which exceed the maximum overrun amount or the
maximum number of overruns, the parent initiates a quick and graceful shutdown. Note
that the executive does not know of the overrun condition until it has finished its
regularly scheduled real-time frame processing.  This means that the overrun detection
is not instantaneous

Through the input file, the user can set overrun limits, use software time checks and
itimers, and use real-time process control features (locking the process in memory,
assigning and locking a process to a processor, setting the process priority, etc.).

By default, a Trick simulation is run non-real-time, that is all jobs will be run as
fast as possible. The frequency of a job specified in the S_define tells Trick when
to run the job within simulation time. Trick keeps track of simulation time by
running each job in order according to its frequency, and accumulating time by adding
each unique frequency to simulation time after all jobs of that frequency have run.
But in non-real-time, Trick simulation time does not correspond to real "wall clock"
time (simulation time is normally much faster than real-time).

Real-time can only occur if you tell Trick to synchronize the simulation time with some
real-time clock. To do this, you first choose a frequency at which Trick should synchronize
to a real-time clock. This frequency is referred to as the real-time frame. Trick
will run all jobs whose frequency \<= real-time frame as fast as possible (this set of
jobs is referred to as an execution frame), then sync to the real-time clock. If,
at this sync point, Trick has to wait because it has finished its execution frame before
the real-time frame, an underrun has occurred, which is the desired behavior. If, at
this sync point, the time to run the execution frame \> real-time frame, an overrun has
occurred, meaning the simulation is running slower than real-time. Trick will immediately
start the next execution frame in an attempt to catch up after an overrun.

Trick provides real-time synchronization using the system clock.
It is also possible to use an external time source instead of the system clock. To do so
you must provide your specific external clock functionality by deriving from Trick's
Clock class. (Trick provides the GetTimeOfDayClock class as a derivative of Clock).
See [Realtime Clock](Realtime-Clock).

A timer may also be used when syncing to real-time during an underrun. Trick provides
this functionality by using a system interval timer or itimer.  It is possible to use
an external timer instead of an itimer. To do so you must provide your specific external
timer functionality by deriving from Trick's Timer class. (Trick provides the ITimer class
as a derivative of Timer).  See [Realtime_Timer](Realtime-Timer).

### User accessible routines

```
int real_time_enable() ;
int real_time_disable() ;
int real_time_restart(long long ref_time) ;
int is_real_time() ;
```

[Continue to Realtime Clock](Realtime-Clock)
