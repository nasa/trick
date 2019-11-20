A JIT input file is a C++ input file that can be used along side a python input file, or even replace the python input file.  The JIT input file includes "S_source.hh" that gives it access to the entire simulation.  The code in a JIT input file is compiled into a shared library during simulation initialization.  The simulation dynamically opens the shared library and runs a specific function called "run_me" that must be defined in the JIT input file.  There are a couple of advantages to this type of input file.

1.  Once compiled, a JIT input file is orders of magnitude faster to execute than a python equivalent.  That is because there is no python is involved, everything is C++.
2. Full C++ functionality.  Our SWIG/python code cannot access everything in the sim, C++ can.
3. Events and malfunctions written in C++ and inputted through a JIT input file will not impact real-time operations as much as python events.
4. A JIT input file can be loaded and executed from a python input file.  you don’t have to choose one type or the other.

An example JIT input file from SIM_Ball++_L1/RUN_cpp_input/input.cpp

```c++
/*
 To use this as an input file include this file on the command line.
 S_main_${TRICK_HOST_CPU}.exe RUN_<dir>/input.cpp
 Trick will compile and link this file at simulation runtime.  The
 run_me function will automatically be executed during the
 input_processor phase of initialization.
*/

// need access to private/protected things? uncomment these.
//#define private public
//#define protected public

#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

extern "C" int run_event_1() {
    message_publish(1, "event 1 fired!\n") ;
    return 0 ;
}

extern "C" int run_event_2() {
    message_publish(1, "event 2 fired!\n") ;
    return 0 ;
}

extern "C" int run_event_3() {
    message_publish(1, "event 3 fired!\n") ;
    return 0 ;
}

extern "C" int run_event_4() {
    message_publish(1, "event 4 fired!\n") ;
    return 0 ;
}

extern "C" int run_event_before() {
    message_publish(1, "event before fired!\n") ;
    return 0 ;
}

extern "C" int run_event_before2() {
    message_publish(1, "event before2 fired!\n") ;
    return 0 ;
}

extern "C" int run_event_after() {
    message_publish(1, "event after fired!\n") ;
    return 0 ;
}

extern "C" int remove_events() {
    event_manager_remove_event(event_manager_get_event("event_3")) ;
    event_manager_remove_event(event_manager_get_event("event_before")) ;
    event_manager_remove_event(event_manager_get_event("event_after")) ;
    return 0 ;
}

/*
Trick will look for the run_me function and execute it during simulation initialization. 
*/
extern "C" int run_me() {

    // modify variables within sim_objects.
    ball.obj.state.input.position[0] = 6.0 ;
    ball.obj.state.input.position[1] = 6.0 ;

    checkpoint_pre_init(true) ;

    // add read events
    jit_add_read( 90.0 , "run_event_1" ) ;
    jit_add_read( 80.0 , "run_event_2" ) ;

    // add cyclic event
    jit_add_event( "run_event_3" , "event_3" , 100.0 ) ;

    // events tied to jobs
    jit_add_event_before( "run_event_before", "event_before", "ball.obj.state_print" ) ;
    jit_add_event_after( "run_event_after", "event_after", "ball.obj.state_print" ) ;

    // add a read event that removes all other events.
    jit_add_read( 150.0 , "remove_events" ) ;

    // tries to add event that does not exist. (currently silents fails)
    jit_add_read( 190.0 , "dne_event" ) ;

    exec_set_terminate_time(200.0) ;

/*
    real_time_enable() ;
    trick_real_time.itimer.enable() ;
    sim_control_panel_set_enabled(true) ;
*/

    return 0 ;
}
```

A JIT input file may be used with a python input file or in place of a python input file.  To use a JIT input file from within a python script use the following calls

```python
# jit_compile_and_run specifies the c++ input file, and an optional function name to run.  The function
# must fit the c++ prototype 'extern "C" int function()'.  If no function name is given, the function
# "run_me" is searched for and ran.
trick.jit_compile_and_run("RUN_cpp_input/input.cpp")
trick.jit_compile_and_run("RUN_cpp_input/input.cpp", "run_event_1")

# a c++ library can be compiled and c++ code executed separately.
trick.jit_compile("RUN_cpp_input/input.cpp")
trick.jit_run("RUN_cpp_input/jitlib/libinput.so", "run_event_1")

# executing a c++ function from input.cpp at a later time.  Note: The add_read call will still be
# processed in python causing a real time hit.
trick.add_read(150, """trick.jit_run("RUN_cpp_input/jitlib/libinput.so", "run_event_1")""")
```

To use a JIT input file in place of the normal python input file, use the C++ input file on the command line.

```
./S_main_<TRICK_HOST_CPU>.exe RUN_cpp_input/input.cpp
```

[Continue to Event Manager](Event-Manager)
