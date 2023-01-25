### Adding Waypoints
This simulation allow for the use of waypoints to create a "flight path" for the aircraft to follow. They are added with a call to:

`dyn.aircraft.add_waypoint( double n, double w )`

*Waypoints added in this way will not be drawn in the variable server client*

### Input File
Waypoints are automatically read into the simulation from the included input file, [`default.waypoints`](Modified\_data/default.waypoints) within the `Modified_data` folder. 

#### Format
Any input file must follow the following format,

```
NORTH_01, WEST_01

NORTH_02, WEST_02
```

Each entry should be a number representing the desired coordinates.