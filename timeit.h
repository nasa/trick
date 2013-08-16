#ifndef TIMEIT_H
#define TIMEIT_H

#include <QDebug>
#include <QString>

class TimeIt
{
public:
    TimeIt();
    void snap(const char* msg)
    {
        qDebug() << msg << stop()/1000000.0;
        start();
    }
    virtual void start() = 0;
    virtual long stop() = 0;   // return microseconds since start
};

#endif // TIMEIT_H
