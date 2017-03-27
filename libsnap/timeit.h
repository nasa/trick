#ifndef TIMEIT_H
#define TIMEIT_H

#include <stdio.h>
#include <QString>

class TimeIt
{
public:
    TimeIt();

    long snap()
    {
        long elapsed = stop();
        start();
        return elapsed;
    }

    void snap(const char* msg)
    {
        fprintf(stderr,"%s %lf\n",msg,stop()/1000000.0);
        start();
    }
    virtual void start() = 0;
    virtual long stop() = 0;   // return microseconds since start
};

#endif // TIMEIT_H
