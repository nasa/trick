# Realtime-Clock

**Contents**

* [Creating a Real-Time Clock Interface with Trick::Clock](#creating-a-clock)<br>
* [Installing a Trick::Clock In Your Simulation](#installing-a-clock)<br>
* [Example Implementation of a Trick::Clock](#example-implemntation)<br>

***

* [**Looking For The Current Simulation Time?**](#looking-for-sim-time)<br>

***

Every real-time simulation requires a clock to which its tasks can be synchronized. By default, a Trick simulation uses the local system clock, by calling *gettimeofday()*. When simulations running on different computers need to cooperate they need to be synchronized to the same clock. So, sometimes we want our simulation to synchronize a to an **external** clock rather than the local one. The ```Trick::Clock``` base class provides a way to create an interface between an external time source, and a real-time Trick simulation.

<a id=creating-a-clock></a>
## Creating a Real-Time Clock Interface with Trick::Clock

The Trick::Clock class is declared in ```trick/Clock.hh```. Deriving a new clock interface class from it requires the following three member functions to be implemented.

***

```c
int Trick::Clock::clock_init();
```
#### 1. clock\_init's Responsibilities
1. If necessary, initialize the time source. This may involve opening and initializing a hardware device, or a network socket connection. 
2. If an error occurs, return a non-zero error code, otherwise call ```set_global_clock()``` and return 0.

***

```c
long long Trick::Clock::wall_clock_time()
```
#### 2. wall\_clock\_time's Responsibilities
1. Get the time from the time source.
2. Return the time in microseconds, or 0 if there **is** an error.

***

```c
int Trick::Clock::clock_stop()
```
#### 3. clock_stop's Responsibilities
1. If necessary, disconnect from the time source.
2. If an error occurs, return a non-zero error code, otherwise return 0.

***

<a id=installing-a-clock></a>

## Installing a Trick::Clock In Your Simulation

By default, Trick uses a derivative of  ```Trick::Clock``` called ```Trick::GetTimeOfDayClock```.

Suppose we've derived a new class ```CuckooClock``` from ```Trick::Clock```, and have created an instance of it:

```c
chalet.my_clock = new CuckooClock();
```

In our simulation's S_define file, we could change the real-time scheduler's clock from the default ```Trick::GetTimeOfDayClock``` to our new ```CuckooClock``` using ```real_time_change_clock()```, declared in ```trick/realtimesync_proto.h```.

```c
void create_connections() {
    chalet.my_clock = new CuckooClock();
    real_time_change_clock(chalet.my_clock);
}
```
We could also change the ```Trick::Clock``` in our input file as follows:

```c
trick.real_time_change_clock(chalet.my_clock)
```

<a id=example-implemntation></a>
## An Example Implementation of a Trick::Clock

The following code is an example implementation of a Trick::Clock, called TPROCTEClock. It provides an interface between Trick's real-time job scheduler and Spectracom's TPRO IRIG-B clock board. This is one of many available time sources that one might use. Note that driver support for timing cards, like the "tpro.h" and "tsync.h" files below, must be acquired from the card's vendor.

<a id=listing_1_Clock-header-file></a>
### TPROCTEClock Header File
```c
/*
PURPOSE: ( TPRO CTE Clock )
*/
#ifndef TPROCTECLOCK_HH
#define TPROCTECLOCK_HH
#include <string>
#include "trick/Clock.hh"
#ifdef _TPRO_CTE
extern "C" {
#include "tpro.h"
#include "tsync.h"
}
#endif

class TPROCTEClock : public Trick::Clock {
    public:
        TPROCTEClock() ;
        ~TPROCTEClock() ;
        int clock_init() ;
        long long wall_clock_time() ;
        int clock_stop() ;
        std::string dev_name ;
    private:
#ifdef _TPRO_CTE
        TPRO_BoardObj *pBoard ;  /* ** board handle */
#endif
    } ;
#endif
```

<a id=listing_2_clock-init></a>
### TPROCTEClock::clock_init

Here, we initialize the timing card by opening the device file, getting a device handle. Then we wait for for it to be available. If an error occurs, return a non-zero error code, otherwise we call ```set_global_clock()``` and return 0.

```c
int TPROCTEClock::clock_init() {
#ifdef _TPRO_CTE
    unsigned char  rv; 
    /* Open the TPRO/TSAT device */
    rv = TPRO_open(&pBoard, (char *)dev_name.c_str());
    /* If unable to open the TPRO/TSAT device... */
    if (rv != TPRO_SUCCESS) {
        printf (" Could Not Open '%s'!! [%d]\n", dev_name.c_str(), rv);
        return (1);
    }   
    /* Wait until this handle is the first user of the device. */
    if (TPRO_setPropDelayCorr(pBoard, NULL) != TPRO_SUCCESS) {
        printf(" Waiting to become first user...\n");
        while (TPRO_setPropDelayCorr(pBoard, NULL) != TPRO_SUCCESS);
    }   
    set_global_clock() ;
    return 0 ; 
#else
    printf("ERROR: Not configured for TPRO CTE card.");
    return -1 ;
#endif
}
```

### TPROCTEClock::wall\_clock\_time
In this function, we get the time from the timing card, and convert it to microseconds before returning it. If the attempt to get the time fails, we return 0.

```c
long long TPROCTEClock::wall_clock_time() {
#ifdef _TPRO_CTE
    TSYNC_HWTimeSecondsObj hwTime;
    /* Send Get Seconds Time message */
    TSYNC_ERROR err = TSYNC_HW_getTimeSec(pBoard, &hwTime);
    if (err != TSYNC_SUCCESS) {
        printf("  Error: %s.\n", tsync_strerror(err));
        return 0;
    }   
    /* If sucessful convert the TPRO time to microsconds. */.
    return hwTime.time.seconds * 1000000LL + (hwTime.time.ns /1000);
#else
    printf("ERROR: Not configured for TPRO CTE card.");
    return 0 ; 
#endif
}
```

### TPROCTEClock::clock\_stop

This function simply closes the handle to the device, and returns 0.

```c
int TPROCTEClock::clock_stop() {
#ifdef _TPRO_CTE
    unsigned char rv ;
    rv = TPRO_close(pBoard);
    /* If unable to close the TPRO/TSAT device... */
    if (rv != TPRO_SUCCESS) {
        printf (" Could Not Close Board!! [%d]\n", rv);
    }
#endif
    return 0 ;
}
```
<a id=looking-for-sim-time></a>
## Looking For The Current Simulation Time ?

Trick::Clock is a class for creating interfaces between timing hardware, or other time sources and Trick's realtime synchronization subsystem. It does not maintain the simulation time. That's maintained by the Trick Executive.

What you may be looking for is the function 

```c
double exec_get_sim_time(void) ;
```

defined in ```exec_proto.h```.



 