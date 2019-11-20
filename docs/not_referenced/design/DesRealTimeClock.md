# Real-Time Clock Design

## Base Real-Time Clock

The base Clock object provides an interface for the following operations.  The
base Clock does not implement the following operations.

1. Initializing the clock hardware
1. Resetting the clock (setting the reference time)
1. Getting the current real time
1. Spinning on the clock<br>
   Spinning on the clock means to continually check the clock waiting for real-time to
   catch up to the desired simulation elapsed time.
1. Stopping the clock

## Gettimeofday Clock

The GetTimeOfDayClock is a clock object that derives from the Base Real-Time Clock.  The
GetTimeOfDayClock uses system clock as the real-time clock.

The GetTimeOfDayClock implements the interface laid out by the base clock class

### Initializing the Clock

@copydetails Trick::GetTimeOfDayClock::clock_init()
Trick::GetTimeOfDayClock::clock_init()

### Resetting the Clock (setting the reference time)

@copydetails Trick::GetTimeOfDayClock::clock_reset()
Trick::GetTimeOfDayClock::clock_reset()

### Getting the Current Real Time

@copydetails Trick::GetTimeOfDayClock::clock_time()
Trick::GetTimeOfDayClock::clock_time()

### Spinning on the Clock

@copydetails Trick::GetTimeOfDayClock::clock_spin()
Trick::GetTimeOfDayClock::clock_spin()

### Stopping the Clock

@copydetails Trick::GetTimeOfDayClock::clock_stop()
Trick::GetTimeOfDayClock::clock_stop()

