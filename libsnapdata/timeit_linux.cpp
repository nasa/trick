#include "timeit_linux.h"

#ifdef __linux

TimeItLinux::TimeItLinux()
{
}

void TimeItLinux::start()
{
    gettimeofday(&_tp_start,NULL);
}

long TimeItLinux::stop()
{
    gettimeofday(&_tp_stop,NULL);

    long elapsed = _tp_stop.tv_usec - _tp_start.tv_usec ;

    if ( _tp_start.tv_sec < _tp_stop.tv_sec ) {
        elapsed += 1000000*(_tp_stop.tv_sec - _tp_start.tv_sec) ;
    }

    return elapsed;
}


#endif
