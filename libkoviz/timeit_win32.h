#ifndef WINTIMEIT_H
#define WINTIMEIT_H

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <limits.h>

#include "timeit.h"

using namespace std;

class TimeItWin32 : public TimeIt
{
public:
    TimeItWin32();
    virtual void start();
    virtual long stop();   // return microseconds since start

private:
    int _gettimeofday(timeval *tp, void *timezone=NULL);
    struct timeval _tp_start;
    struct timeval _tp_stop;

};

#endif

#endif // WINTIMEIT_H
