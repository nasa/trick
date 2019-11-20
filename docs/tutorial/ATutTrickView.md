| [Home](/trick) → [Tutorial Home](Tutorial) → Viewing Real-Time Data |
|-------------------------------------------------------------------|

<!-- Section -->
<a id=viewing-real-time-data-with-trick-view></a>
## Viewing Real-Time Data with Trick View (TV)

Trick View (TV) is an application for viewing/setting simulation variables in
real-time. TV can be launched from the input file, or may be launched directly
from the sim control panel. You can also leave TV running between simulation
runs. In this case You will need to click the "Connect" button in the lower right
corner of the TV GUI to re-connect to the simulation.

#### TV Without An Input File
1. Fire up the simulation:

   ```bash
   % cd $HOME/trick_sims/SIM_cannon_analytic
   % ./S_main*exe RUN_test/input.py &
   ```

1. Start TV by either clicking the blue TV icon button or choosing
the sim control panel menu option: **Actions->Start TrickView**. The TV
GUI should pop up.

1. In the **Variable Hierarchy Tree** pane, on the left side of the
GUI, double-click `dyn`, and then double-click `cannon`.

1. Choose some variables to view. Double-click on `pos[2]`, and
`vel[2]`. These variables will be added to the **Variable Table** pane on the
right.

1. Since we have gone to the trouble, go ahead and save these
selections.

    * Click the **Save** button on the TV toolbar, or select **File->Save** from
    the main menu.
    * Save to the `SIM_cannon_analytic` directory, and name the file "TV_cannon".

1. On the sim control panel, choose **Actions->FreezeAt**. Enter in a
time of 2.0 seconds. Click **OK**.

1. On the sim control panel, click the **Start** button to put the
simulation into action. Notice that TV parameter values have changed. When the
simulation time reaches 2.0 seconds, the cannon ball position will be [86.17, 30.33].

1. In the Trick View variable table, select `dyn.cannon.pos[0]` and
`dyn.cannon.pos[1]`. You can use SHIFT-click to select both. With these
variables high-lighted, select **Actions->Strip Chart** on the menu bar. The
Strip Chart GUI should then appear, and represent each of the two variables as a
squiggly line, and unique color of its own. Also verify that the two values of
dyn.cannon.pos as represented in TV, and Stripchart agree.

1. Now force the Y position of the cannonball to drop to 10
meters. To do this, click on the dyn.cannon.pos[1] variable on the TV Parameter
table. Replace 30.325... with 10.0 as a new position. Hit **Enter** to set the
variable with the new value. The stripchart should show the drop from 30 meters
to 10 meters.

1. Notice that dyn.cannon.vel[0] is 43.30... meters per second. To
view it in feet per second:
    * Left Click on the variable dyn.cannon.vel[0] on the Variable table.
    * Left Click on the "m/s" in the Unit column to bring up a drop-down list.
    * Select **ft/s**. Notice that the value of dyn.cannon.vel[0] changes to
    142.06... ft/s.

1. Resume the simulation run by clicking the **Start** button on the
sim control panel. Notice that the trajectory assumes its predetermined path.
This is because we are giving the cannonball a position as a function of time.

#### TV With An Input File
If this simulation were run over and over, it would be laborious to
clickety-click the variables each time. It would be advantageous to use the
TV_cannon file we saved off in the last step. There are two ways to do this.

##### Loading A TV Input File Directly From TV

1. Fire up the cannon.

```bash
% cd $HOME/trick_sims/SIM_cannon_analytic
% S_main*exe RUN_test/input.py
```

1. Choose "Actions->Start Trick View" from sim control panel.

1. Click the file **Open** or **Open** with a blue circle icon button on
the TV toolbar or select File->Open or File->Open & Set from the main menu.

1. Select the TV_cannon file saved off in the last run. The Parameter
table on the right is cleared and replaced with all the saved variables.

1. If "Set" is chosen for opening a TV file, all saved values are
restored as well. Otherwise, only variables are loaded to the Parameter table.
If only "Set" is selected, corresponding vaiable values are restored without the
Parameter table being changed.
This simulation is simple since we have a limited number of parameters. The
streamlining is more pronounced when the simulation has thousands of variables,
in which the process for selecting variables would become awfully repetitious.

##### Allowing The Simulation To Load A TV Input File
It even gets tiresome clicking the TV input file from TV. The following example
shows how to configure the simulation to automatically launch the TV with the
parameters of interest. The syntax for this file is similar to the stripchart
input file.
Again, we need to incorporate the TV input file into our ever expanding
simulation input file.

```python
execfile("Modified_data/realtime.py")
execfile("Modified_data/cannon.dr")

trick.trick_view_add_auto_load_file("TV_cannon.tv")
trick.stop(5.2)
```
**Listing 9a - Simulation TV Input File - input.py**

```bash
% cd $HOME/trick_sims/SIM_cannon_analytic/RUN_test
% vi input.py <edit and save>
```

You may now run the sim and verify that TV pops up automatically.

Congratulations, you have finished the basic Trick tutorial!

[Next Page](ATutNumericSim)
