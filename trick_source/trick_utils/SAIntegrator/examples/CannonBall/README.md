# CannonBall

This is an example of using the RK2Integrator to create a simple cannon ball simulation. We also use a rootfinder with out integrator to detect contact with the ground, and bounce the cannonball.

```
$ make
$ ./CannonBall > cannon.csv
$ python plot_trajectory.py
```

![Cannon](images/Cannon.png)
