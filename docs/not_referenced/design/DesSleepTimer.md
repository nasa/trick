# Sleep Timer Design

After all jobs in an execution frame have run, the default behavior (the timer is disabled)
is that the simulation will spin on the clock until real-time has caught up to simulation time.
Enabling the sleep timer gives control to the timer's pause() function, in which it may
release the processor and regain control when the timer elapses.

## Base Sleep Timer

The base Timer object provides a common interface for all Timers for the
following functions.

1. Enabling the Timer
1. Disabling the Timer
1. Getting the enabled/disabled status

The base Timer object provides an interface for the following operations.  The
base Timer does not implement the following operations.

1. Initializing the Timer hardware
1. Setting the frame time for timer
1. Starting the timer
1. Resetting the timer
1. Pausing for the timer to expire
1. Stopping the timer
1. Shutting down the Timer hardware

## ITimer

The ITimer is a Timer object that derives from the Base Sleep Timer.  The
ITimer uses system itimer functionality to allow the simulation to release the
cpu while waiting for the timer to expire.  When the itimer expires it uses
the SIGALRM signal to set a semaphore.  When the semaphore is set the simulation
wakes up.

The itimer does not expire exactly at the desired period.  It actually exipres after
the desired period by up to 1ms.  To compensate for the delay we subtract 2ms from
the desired period.  The RealtimeSync class "spins" on the clock the remaining
1-2ms the timer does not sleep for.

The ITimer implements the interface laid out by the base Timer class

### Initialization

@copydetails Trick::ITimer::init()
Trick::ITimer::init()

### Setting the Frame Time

@copydetails Trick::ITimer::set_frame_times()
Trick::ITimer::set_frame_times()

### Starting the timer.

@copydetails Trick::ITimer::start()
Trick::ITimer::start()

### Restarting the timer.

@copydetails Trick::ITimer::reset()
Trick::ITimer::reset()

### Pausing for the Timer to Expire

Pausing for the timer has 2 parts.  The first is the signal handler that posts
a semaphore in response to a SIGALRM signal.  The signal handler is assigned
to SIGALRM during the initialization routine.  The second is the pause routine
that waits for the semaphore to be posted.

#### Signal Handler

@copydetails Trick::ITimer::it_handler()
Trick::ITimer::it_handler()

### Pause for Semaphore

@copydetails Trick::ITimer::pause()
Trick::ITimer::pause()

### Stopping the timer.

@copydetails Trick::ITimer::stop()
Trick::ITimer::stop()

### Shutting Down the Timer

@copydetails Trick::ITimer::shutdown()
Trick::ITimer::shutdown()
