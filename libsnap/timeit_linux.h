#ifndef TIMEIT_LINUX_H
#define TIMEIT_LINUX_H

#ifdef __linux
#include "timeit.h"
#include <sys/time.h>
#include <stddef.h>

using namespace std;

class TimeItLinux : public TimeIt
{
public:
    TimeItLinux();
    virtual void start();
    virtual long stop();   // return microseconds since start

private:
    struct timeval _tp_start;
    struct timeval _tp_stop;

};

#endif

#endif // TIMEIT_LINUX_H
