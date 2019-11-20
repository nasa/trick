| [Home](/trick) → [Tutorial Home](Tutorial) → Running Real-Time |
|--------------------------------------------------------------|

<!-- Section -->
<a id=running-real-time></a>
## Running Real-Time

Recall that the cannonball run was 5.2 seconds, yet when the simulation ran, it
was done in a flash of CPU time. This section will add real-time synchronization.

### Making A Real-time Input File

Similar to making a data recording file, a real-time input file is needed.

```python
trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()
trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)
trick.sim_control_panel_set_enabled(True)
```

**Listing 10 - Real Time File `realtime.py`**

```bash
% cd $HOME/trick_sims/SIM_cannon_analytic/Modified_data
% vi realtime.py <edit and save>
```

Here is a brief explanation.

`trick.frame_log_on()` - tells the simulation to log the performance of the
simulation (i.e., how it is running with respect to the real-world time).

`trick.real_time_enable()` - tells the simulation to run in real-time
synchronization.

`trick.exec_set_software_frame(0.1)` - tells the simulation the frequency of
the "heartbeat" of the simulation. If the simulation were sending out packets of
information to a graphics server, the "heartbeat" might be 50 HZ (0.02). Trick
synchronizes with the system clock on multiples of the rt_software_frame. If it
is beating the system clock, it pauses. If it is falling behind, it registers
"timeouts" and attempts to catch up.

`trick.itimer_enable()` - allows other processes to run while Trick is
waiting for the beginning of the next software frame to start the simulation
jobs. If interval timers are not used, Trick will spin waiting for the next beat.

`trick.exec_set_freeze_command()` - brings up the simulation in a frozen
(non-running) state.

`trick.exec_set_enable_freeze()` - allows the user to toggle the simulation
from a frozen state to a running state at will.

`trick.sim_control_panel_set_enabled(True)` or
`simControlPanel = trick.SimControlPanel() & trick.add_external_application(simControlPanel)`
 - brings up the simulation control panel GUI.

The `realtime.py` file must be included in the RUN_test/input.py file. When
finished, the latest version of the input file should look like the following:

```python
execfile("Modified_data/realtime.py")
execfile("Modified_data/cannon.dr")
trick.stop(5.2)
```

```bash
% cd $HOME/trick_sims/SIM_cannon_analytic/RUN_test
% vi input.py <edit and save>
```

### Using The Sim Control Panel

Fire up the cannonball again.

```bash
% cd $HOME/trick_sims/SIM_cannon_analytic
% ./S_main*.exe RUN_test/input.py &
```

The **simulation control panel** should popup now. In the `realtime.py` file, we
instructed Trick to start in a **freeze** state. Currently, the ball is sitting
in the base of the barrel. To fire it, the **Start** button must be clicked on
the simulation control panel in the **Commands** box.

1. Click **Start** on simulation control panel. The simulation will
run in sync with real-time.

1. Once the simulation is finished, click the **Exit** button.
Some items to note about the simulation control panel for your future use:
    * You may freeze the simulation at any point, and then restart.
    * You may freeze the simulation, then "dump a checkpoint" of the current
    state. This state is reloadable (i.e., you may jump to a previous/future
    point in the sim by loading the appropriate checkpoint).
    * You may toggle between real-time and non-real-time.
    * If the simulation lags behind real-time, it will log these as overruns
    (does not complete all jobs during the software frame) and display them in
    the tiny box next to the simulation name. If the simulation overruns, the
    sim will run as fast as it can "to catch up" to where it should be.
    * Using the File menu at the top, you may set a freeze point in the future.

---

[Next Page](ATutTrickView)
