
A realtime clock is an optional feature for Trick simulations.

The Trick::Clock base class provides Trick with a consistent interface to different
realtime clocks.  The base class provides a common set of routines called by
Trick::RealtimeSync to get and reset the realtime clock.

Trick provides built in support for using the system clock (Trick::GetTimeOfDayClock).

Trick also provides built in support for using Spectracom's TPRO IRIG-B clock board,
also referred as a Central Timing Equipment (CTE) board.  TPRO CTE support is off
by default.  To enable TPRO CTE support Trick must be built with TPRO driver support.
Go to http://www.spectracomcorp.com for driver support for your particular board.
To enable TPRO CTE support, run ${TRICK_HOME/configure --tprocte=/paht/to/tpropci.
Recompile Trick after the configure script.

### Creating a User Defined Real-Time Clock

Clocks may be created by users.  To create a new clock, the class must inherit from
the Trick::Clock.  Trick::Clock is an abstract class with several methods for the user
to implement.  These methods provide the hardware specific code to initalize, access, and
shutdown clock hardware. See [${TRICK_HOME}/trick_source/sim_services/Clock](../blob/master/trick_source/sim_services/Clock) for examples
to how we implemented the GettimeofDay and the TPRO_CTE clock.  This is the list of
routines that must be implemented by the user:

##### Trick::Clock::clock_init()

Opens the clock hardware, initializes the clock, etc.

##### Trick::Clock::wall_clock_time()

Gets the wall clock time.  Returns the time in integer number of tics of 1us.

##### Trick::Clock::clock_spin(long long req_time)

Waits for real-time to catch up to the requested simulation time.  Polling or waiting
for an interrupt are typical methods for spinning.

##### Trick::Clock::clock_stop()

Shuts down clock hardware, closes devices, etc.

### Specifying the Real-Time Clock

Trick instantiates an instance of the Trick::GetTimeOfDayClock by default.
Trick includes a realtime clock based on a TPRO timing card.  TPRO support must be enabled when compiling Trick core.  A Trick::TPROCTEClock class may be included in an S_define file with <code>##include "sim_objects/TPROClock.sm"</code>.

Trick defaults to the Trick::GetTimeOfDay clock. To switch to the TPRO CTE clock
or a user defined clock use the following line in the Python input file.

```
# Changes to the TPRO CTE clock
trick.real_time_change_clock(trick_tpro.tpro_cte)
# Changes to a user defined clock
trick.real_time_change_clock(<user defined clock>)
```

### Accessing the Real-Time Clock

Typically, users will not have to access the real-time clock directly, the
Trick::RealtimeSync uses the real time clock to synchronize to real time.  However
there are several user accessible routines to control the clock and get the status.

```
#include "trick_source/sim_services/Clock/include/clock_proto.h" for these routines
```

#### Clock Time

```
long long clock_time() ;
```

```
# Python access
trick.clock_time()
```

Gives you real-time referenced from simulation start.  The returned value is in tics.
A tic is set amount of time, set by the Trick::Executive, usually 1us.

#### Wall Clock Time

```
long long wall_clock_time() ;
```

```
# Python access
trick.wall_clock_time()
```

Gives you the actual time from the clock.  The returned value is in tics
A tic is set amount of time, set by the Trick::Executive, usually 1us.

#### Clock Spin

```
int clock_spin(long long ref) ;
```


```
# Python access
trick.clock_spin(<ref_time>)
```

Calling clock spin will cause the simulation to enter a loop and wait until
realtime catches up to simulation time.  This is not typically called by
users.

#### Reset Clock Reference

```
long long clock_reset(long long ref) ;
```

```
# Python access
trick.clock_reset(<ref_time>)
```

Resetting the clock synchronizes the "reference" time to the real-time clock.
The reference time is typically the current simulation time.

#### Manually Setting Clock Reference

```
int clock_set_reference(long long wall_clock_time) ;
```

```
# Python access
trick.clock_set_reference(<wall_clock_time>)
```

This manually synchronizes the clock's reference to the incoming wall clock time.
This routine is used internally to synchronize the simulation time to a whole
second multiple of the real time clock.

#### Getting the Clock Ratio

```
double clock_get_rt_clock_ratio() ;
```

```
# Python access
trick.clock_get_rt_clock_ratio()
```

The realtime clock has the ability to run at a ratio to realtime.  This routine
gets the current set ratio.

#### Setting the Clock Ratio

```
int clock_set_rt_clock_ratio(double in_rt_clock_ratio) ;
```

```
# Python access
trick.clock_set_rt_clock_ratio()
```

The realtime clock has the ability to run at a ratio to realtime.  This routine
sets the current set ratio.  A simulation may not be able to run fast enough to
honor the set ratio.  In this case the simulation will run as fast as possible.

[Continue to Realtime Timer](Realtime-Timer)
