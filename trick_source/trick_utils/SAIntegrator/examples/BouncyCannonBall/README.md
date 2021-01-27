# BouncyCannonBall

The BouncyCannonBall program adds dynamic events to the Cannonball simulation.
using a rootfinder with our integrator to detect contact with the ground, and
bounce the cannonball.

For each numerical integration time-step, the simulation program prints:

1. time (s)
2. 2D position vector (m)
3. 2D velocity vector (m/s)

to ```stdout```, in Comma Separated Values (CSV) format.

### Building & Running the Simulation Program

Generate the results as follows:

```
$ make
$ ./BouncyCannonBall > cannon.csv
```
### Plotting the Results
The Python script, ```plot_trajectory.py``` is provided to plot the results
in ```cannon.csv``` using (Python) matplotlib.

Plot the cannon ball trajectory as follows:

```
$ python plot_trajectory.py
```

![Cannon](images/Cannon.png)

