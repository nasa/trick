# SIM_WaterClock

---

SIM_WaterClock simulates a simple water clock. A water clock is a clock that measures time utilizing the flow of water. There exists different designs for water clocks, but this one uses a basic two bucket design.

The first bucket is called the intake bucket. The intake bucket receives water from an external source of variable flow rate. In the real world this might be piped in from a stream for example. In this sim, the water source (input_flow) is a variable range from 0 to 10 m/s<sup>3</sup> that can be arbitrarily adjusted via the GUI. The intake bucket contains two spouts, a clock spout and an overflow spout. Both spouts are cylinders defined by their width and height on the intake bucket. The clock spout pours water into the second bucket (timer bucket), while the overflow spout empties water into the void.

In an ideal water clock, by balancing the dimensions of the intake bucket, the clock spout, overflow spout, and the range of the input flow rate, the flow rate of the clock spout (and therefore the input flow rate of the timer bucket) will be constant. Given a constant flow rate into the timer bucket, we can correlate water level with time passed. 

Assumptions:
* For the purpose of determining if a pipe is submerged, it will be treated as a single point at the center of the pipe.
* When the timing bucket is filled, a magical siphon will drain it instaneously. This prevents the need to simulate the recalibration of the clock.
* Buckets and spouts are perfectly cylindrical.

### Building the Simulation
In the SIM\_waterclock directory, type **trick-CP** to build the simulation executable. When it's complete, you should see:

```
Trick Build Process Complete
```
Now **cd** into models/graphics/ and type **mvn package**. This builds the display client for the simulation.

### Configurable Parameters
The following parameters are meant to be configured by the user. They consist of the dimensions of the buckets, the dimensions and placement of the intake bucket spouts, and the number of time ticks.

Variable                                                              | Type           | Units
----------------------------------------------------------------------|----------------|-------
dyn.waterclock.intake_bucket_depth                                    | double         | m
dyn.waterclock.intake_bucket_diam                                     | double         | m
dyn.waterclock.intake_overflow_height                                 | double         | m
dyn.waterclock.intake_overflow_diameter                               | double         | m
dyn.waterclock.intake_clock_spout_height                              | double         | m
dyn.waterclock.intake_clock_spout_diameter                            | double         | m
dyn.waterclock.timer_bucket_depth                                     | double         | m
dyn.waterclock.timer_bucket_diam                                      | double         | m
dyn.waterclock.total_ticks                                            | int            | --
