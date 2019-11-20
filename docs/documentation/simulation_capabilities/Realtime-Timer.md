
A realtime sleep timer is an optional class for Trick simulations.

The Realtime Sleep Timer allows the simulation to sleep in case of an frame underrun.
This is desireable to allow other processes to use the CPU during times where the
simulation would not be doing anything.

Trick provides a Sleep Timer based on the system itimer to sleep during
simulation inactivity.

The user has the ability to enable and disable the sleep timer from the input file.

```
trick.itimer_enable()
trick.itimer_disable()
```

[Continue to Real-time Injector](Realtime-Injector)
