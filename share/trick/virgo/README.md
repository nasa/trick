# VIRGO

VIRGO: **V**ersatile **I**maging and **R**endering for **G**alactic **O**perations. A Practical, Analytical, and Hardworking 3D Visualization tool for simulation data leveraging python-VTK.  Designed by and for engineers at NASA JSC (Johnson Space Center), VIRGO gives users insight into simulation data when typical 2D plots are insufficient. This project is developed specifically for engineers in the GNC division of the Engineering Directorate but aims to provide functionality for the [Trick](https://github.com.nasa/trick)-using community and beyond. 

![virgo1](images/doc/virgo_example1.png)
![virgo2](images/doc/virgo_example2.png)

VIRGO intends to be an extensible, well-tested, and robustly documented python module which can consume simulation data in different ways. However, **VIRGO is currently in *alpha* status which means it has not yet been verified or validated in any way. Use caution when using VIRGO visualizations to draw conclusions about your engineering application. It is highly recommended that you cross-reference what you are seeing with other plotting or visualization tools before using a VIRGO scene to make an engineering decision.**

## A simple VIRGO example

The simplest data source supported is a typical `*.csv`  (comma-separated-value) text file containing a time history of object positions in 3D space. For example, imagine you had a file `log_state.csv` that describes the motion of a satellite in space and looks like this:

```csv
time (s),  position[0] (m),  position[1] (m),  position[2] (m),
     0.0,              0.0,              0.0,              0.0, 
     1.0,              5.0,              5.0,              5.0, 
     2.0,             10.0,             10.0,             10.0, 
     3.0,             20.0,             20.0,             20.0, 
     4.0,             30.0,             30.0,             30.0, 
```
The first line describes what the variables are and their `(units)`. All other lines provide monotonically increasing time values and position values at those times.  VIRGO can read this file and playback the position of the object described by the `position` array over the timeframe 0.0 - 4.0 seconds. In this example the x-position is `position[0]`, y-position is `position[1]`, and z-position is `position[2]` . In VIRGO terminology this file is considered a simple **data source**.

The **scene** defines what **actors** (the rendered objects) look like and links that object to a **data source**. VIRGO defines scenes using python dictionaries which are easily expressed as YAML files that look like this:

```yaml
# This section defines what objects (actors) are in the scene
# In this example there is only one actor named satellite
actors:
  satellite:           # User-defined name for this actor
    mesh: VIRGO_PREFAB:cube  # 3D representation of the object, a cube
    scale: 1.0         # The cube is 1 unit wide/tall/deep
    # This subsection defines what data source alias drives the actor
    driven_by:
      time: sim_time   # Time is defined as sim_time: from section below
      pos: sat_pos     # Position is defined as sat_pos alias

# This section defines what to read from a trickpy data source
data_source:
  trickpy:
    sim_time: 
      group: state # 'state' comes from log_state.csv
      var: time    # The name for time (1st column 1st row of log_state.csv)
    sat_pos:       # User-defined alias for satellite x,y,z position values
      group: state       # 'state' comes from log_state.csv
      var: position[0-2] # Variables comprising sat_pos alias
```
This tiny example gives you a sense of what VIRGO can do. Any integer or floating point data can be consumed and used in VIRGO in various ways. Here are a few things you can do with VIRGO:
1. Drive positions and orientations of **VIRGO actors** with respect to inertial space and/or relative to another actor. There is no limit to the number of actors in the scene beyond what your hardware can handle.
2. Use prefabricated meshes (cube, sphere, cylinder, etc.) to represent actors or provide your own mesh with `*.stl` or `*.obj` files
3. Visualize vectors that change over time in 3D space using **VIRGO vectors**
4. Display simulation data as text in the 3D scene using **labels**
5. Show where a **VIRGO actor** has been by enabling trails
6. Play, pause, and adjust playback speed
7. Step forward and backward through data while paused
8. Choose from multiple lighting modes including light from a sun positioned by simulation data


## Module Dependencies

This module requires python3.11 or later and the `pip` packages listed in `requirements.txt`. To pull down these packages to a standard python3.11+ virtual environment run the following in the directory you wish to create the `.venv`:
```bash
# Create a python3 virtual environment
python3.11 -m venv .venv && source .venv/bin/activate 
# Install virgo dependencies
pip3 install --upgrade pip && pip3 install -r requirements.txt
```
Once the `.venv` is created, you can source the environment in any shell before running scripts using VIRGO:

```bash
source .venv/bin/activate
```
## Terminology

* **A VIRGO Scene** is a python dictionary (often expressed as a YAML file) that fully articulates the details of the 3D scene that will be rendered and displayed to the user. This is the main method of configuration provided to the users of VIRGO and includes a list of `actors:`, `vectors:`, `frames:` as well as settings within the scene like lighting, data sources, labels, etc.
* **VIRGO Actors** are 3D objects rendered in the scene and are defined by the `actors:` section of the scene dictionary. They can be positioned and oriented statically in the scene or driven by simulation data.
  * **VIRGO Vectors** are **VIRGO Actors** that must be represented as an arrow in 3D space. Vectors are defined in the `vectors:` section of the scene dictionary and accept a position value which defines the tip of the vector to be drawn. 
  * **VIRGO Frames** are **VIRGO Actors** that have no mesh geometry. You can think of them as a reference frame positioned and oriented relative to some other reference frame or the world origin if the frame has no parent.
* **VIRGO Nodes** define the scene graph tree by managing **VIRGO Actors, Frames, and Vectors** as well as their position and orientation with respect to their parent or world coordinates if no parent is defined. Every actor, frame, and vector is automatically assigned and parented to a unique node of the same name. When objects move in a VIRGO scene it's because their underlying **VIRGO Node** is moving.  
* **Parents** allow nodes containing actors, frames, and vectors, to be defined relative to another actor, frame, or vector, by defining a `parent:` relationship. If a node has a parent that means all transformations on that node will be performed relative to that parent. Nodes and their parental relationships fully define one or more [directed acyclic graph trees](https://en.wikipedia.org/wiki/Tree_(graph_theory)) which contain all the information needed to determine world position/orientation of any actor in the scene.
* **Root nodes** are nodes have no parents and therefore are positioned and oriented with respect to world coordinates (often considered inertial space)
* **Data Sources** provide engineering data associated with actors, frames, and vectors. Typically this amounts to variable values at specific simulation times, for example the position of a cube over a 10 second period.
* **Labels** allow the user to display text positionally within the 3D scene. Each **VIRGO Actor, Vector, and Frame** may optionally define `labels:` to render static text or information from a **data source** as the user sees fit.

### Actors

Details TBD

### Frames

Details TBD

### Vectors

Details TBD

## Lighting

Details TBD

## Examples

Standalone examples can be found under the `examples/` directory. 

Details TBD.

## Configure Scenes with a YAML file

[YAML](https://yaml.org/) provides a handy way to define python dictionaries and track them as code in a human-readable form. Note that YAML is not required to use VIRGO but you will need to provide VIRGO with a **scene dictionary** which fully defines the scene that will be rendered.

**TODO verbose explanation of the YAML file format goes here**

## Contributing

Details TBD

