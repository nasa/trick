# Pool Table Display

Uses the Libigl library for visualization. Connects to SIM_Billiards


## Compile

libigl must be downloaded from Github : https://github.com/libigl/libigl
Clone it somewhere accessible, like your home directory. It does not need to be compiled.

libigl depends on Eigen3, which also need to be downloaded and installed : https://eigen.tuxfamily.org/index.php?title=Main_Page


To compile:

    mkdir build
    cd build
    cmake ..
    make

## Run

From within the `build` directory:

    ./PoolTableDisplay

