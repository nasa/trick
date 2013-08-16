#ifndef TIMEIT_H
#define TIMEIT_H

class TimeIt
{
public:
    TimeIt();
    virtual void start() = 0;
    virtual long stop() = 0;   // return microseconds since start
};

#endif // TIMEIT_H
