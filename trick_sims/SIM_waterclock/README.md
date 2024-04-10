# SIM_WaterClock

---

SIM_WaterClock simulates a simple water clock. A water clock is a clock that measures time utilizing the flow of water. There exists different designs for water clocks, but this one uses a basic two bucket design.

The first bucket is called the intake bucket. The intake bucket receives water from an external source of variable flow rate. In the real world this might be piped in from a stream for example. In this sim, the water source (input_flow) is a variable range from 0 to 10 liters/s that can be arbitrarily adjusted via the GUI. The intake bucket contains two spouts, a clock spout and an overflow spout. Both spouts are cylinders defined by their width and height on the intake bucket. The clock spout pours water into the second bucket (timer bucket), while the overflow spout empties water into the void.

In an ideal water clock, by balancing the dimensions of the intake bucket, the clock spout, overflow spout, and the input flow rate, the flow rate of the clock spout (and therefore the input flow rate of the timer bucket) will be constant. Given a constant flow rate into the timer bucket, we can correlate water level with time passed. 

Assumptions:
* For the purpose of determining if a pipe is submerged, the pipe will be treated as a single point at its center.
* When the timing bucket is filled, a magical siphon will drain it instaneously. This prevents the need to simulate the recalibration of the clock.
* Buckets and spouts are perfectly cylindrical.

### A Brief History
The water clock (or clepsydra to the ancient Greek) is a device used to measure time through the displacement of water. It is not known precisely when the earliest water clocks were developed, but they have been discovered to exist in various parts of the world as early as the 16th century BC. These early designs were relatively simple compared to later designs, consiting of a bowl with markings to indicate time passing as the bowl either filled or drained.

Water clocks remained a prominent timekeeping device until the 17th century when better methods of timekeeping were developed. Over those many centruies, different cultures innovated on the design of the early water clocks. Of particular note were the contributions of the Greek inventor Ctesibius in the 3rd century BC. Ctesibius innovated on the water clock by making it largely automatic. Prior to Ctesibius, water clocks needed to be consistently refilled/emptied by hand. Furthermore, if a water clock had indicators marking the position of the sun, they would need to be manually adjusted for different seasons. Ctesibius automated both these processes. 

Ctesibius automated the draining process with a siphon akin to Pythagorean cup. This siphon would empty over a water wheel, which powered a series of gears that would turn a cylinder. The cylinder had a row of irregular rings drawn around it, meant to correlate with the position of the sun throughout the seasons. Each day, as the water clock would fill it would accurately indicate the position of the sun, drain itself at the end of the day, and adjust the cylinder to be accurate for the next day.

### Building the Simulation
After building trick, in the SIM\_waterclock directory run the command **trick-CP** to build the sim. When it's complete, you should see:

```
Trick Build Process Complete
```
To build the graphics client **cd** into models/graphics/ and run **mvn package**. This isn't necessary for the sim to run, but will provide a visual display of the water clock.

### Running the Simulation
In the SIM_waterclock directory:

```
% S_main_*.exe RUN_test/input.py
```
The Sim Control Panel, and a GUI called "Water Clock" should appear.

![Water Clock graphics client at the start of a run.](images/WaterClockStart.png)

Click the Start on the Trick Sim Control Panel. The Water Clock should begin to fill up.

![Water Clock graphics client mid-run.](images/WaterClockRunning.png)

The only control on the GUI client is a slider which changes the flow rate of water into the water clock. It can be set anywhere from no flow up to 10 liters per second (10,000 cm^3^/s).

By default, the water clock will count out 60 "ticks" at a little over 1 second a tick. See if you can clibrate the water clock to count out a proper minute (1 second a tick).

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
