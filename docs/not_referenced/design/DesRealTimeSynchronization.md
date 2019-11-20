# Real-Time Synchronization Design

A simulation is "real-time" when it can consistently and repetitively execute
its scheduled math models to completion within some predetermined interval
time frame for an indefinite period of time. This predetermined interval
time frame is referred to as the real-time frame.  In this design the real-time
frame is set equal to the simulation software frame.  Real-time is achieved
by periodically pausing simulation execution at the end of each real-time
frame.  If simulation execution has not finished by the end of each real-time
frame then this is an overrun.  The real-time synchronization capability has
several user settable options when a simulation overruns.  If simulation
execution finishes before the real-time frame expires, this is an underrun.  If
the simulation is underruning, the real-time synchronization capability allows
the simulation to "sleep" while waiting for the real-time frame to expire.

Real-time synchronization defaults off when initializing the simulation.
To enable real-time synchronization users or models may call enable() at any
time during the simulation.  To exit real-time synchronization users or
models may call disable() at any time during the simulation. The actual
entering or exiting real-time happens at the end of the frame.

Listed below are the various modes of operation in a simulation and the
responsibilities of the real-time synchronization class in each.

## Pre-Initialization

During pre-initialization the user may specify the behavior of the real-time
synchronization class during overruns and underruns.

1. Set real-time synchronization enabled or disabled.
1. Set real-time sleep timer enabled or disabled.
1. Set the maximum number of consecutive overruns allowed. (default 100,000,000)
1. Set the maximum time allowed for single overruns (default 1e37 seconds)
1. Set action taken when maximum overruns achieved to freeze/terminate or
  terminate immediately (default terminate immediately)

## Initialization and Restart

The real-time initialization and restart jobs are designed to be the last
jobs run before exiting initialization or restart mode.

Initialize the underlying hardware.

@copydetails Trick::RealtimeSync::initialize(long long , long long )
Trick::RealtimeSync::initialize(long long , long long )

## Start Real-Time

@copydetails Trick::RealtimeSync::initialize(long long , long long )
Trick::RealtimeSync::initialize(long long , long long )

## End of Frame

At the end of the real-time frame (also the software frame) the
real-time synchronization class will determine if the simulation is
underruning or overrunning and take actions to keep simulation time
and real-time in sync.

@copydetails Trick::RealtimeSync::rt_monitor( long long )
Trick::RealtimeSync::rt_monitor( long long )

## Freeze Mode

### Freeze Initialization

During freeze initialization the real-time synchronization class resets the real-time
frame to the freeze frame and restarts the real-time clock and timer.

@copydetails Trick::RealtimeSync::freeze_init( long long )
Trick::RealtimeSync::freeze_init( long long )

### Freeze Loop

During freeze loop processing the real-time synchronization pauses the freeze loop.
The pause is released when real-time equals the freeze loop period.  No checks are
done for overrunning or underruning a freeze period.

@copydetails Trick::RealtimeSync::freeze_pause( long long )
Trick::RealtimeSync::freeze_pause( long long )

### UnFreeze

During unfreeze the real-time synchronization class resets the real-time
frame to the software frame and restarts the real-time clock and timer.

@copydetails Trick::RealtimeSync::unfreeze( long long , long long )
Trick::RealtimeSync::unfreeze( long long , long long )

## Shutdown Mode

During shutdown, the real-time synchronization class stops the clock and sleep
timer hardware.

@copydetails Trick::RealtimeSync::shutdown()
Trick::RealtimeSync::shutdown()
