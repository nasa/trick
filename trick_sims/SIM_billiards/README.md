# Billiards Sim

Playable simulation of a billiards table. Includes friction, impulse collision handling, and cue forces currently. 

Angular forces on cue ball, user control of parameters, and detailed physics writeup coming soon :)


<br><br>

# Pool Table Display

Located in models/graphics/cpp, uses the Libigl library for visualization. 

## Compile

libigl must be downloaded from Github : https://github.com/libigl/libigl

Clone it somewhere accessible, like your home directory. It does not need to be compiled.

libigl depends on Eigen3, which may (or may not) need to be downloaded and installed : https://eigen.tuxfamily.org/index.php?title=Main_Page

To compile:
```
cd models/graphics/cpp
mkdir build
cd build
cmake ..
make
```

The provided input files will automatically run and connect to the display.