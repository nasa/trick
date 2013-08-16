#include "timeit_win32.h"

#ifdef __WIN32__

TimeItWin32::TimeItWin32()
{
}

void TimeItWin32::start()
{
    _gettimeofday(&_tp_start);
}

long TimeItWin32::stop()
{
    _gettimeofday(&_tp_stop);

    long elapsed = _tp_stop.tv_usec - _tp_start.tv_usec ;

    if ( _tp_start.tv_sec < _tp_stop.tv_sec ) {
        elapsed += 1000000*(_tp_stop.tv_sec - _tp_start.tv_sec) ;
    }

    return elapsed;
}

int TimeItWin32::_gettimeofday(struct timeval* tp, void* timezone)
{
    if ( timezone != NULL ) {
        cerr << "WinTimeIt::_gettimeofday(): no support for timezone"
             << endl;
        return(-1);
    }

    static BOOL is_first = true;
    static LONGLONG freq ;
    static LONGLONG sec0 = 0;
    static LONGLONG usec0 = 0;

    LARGE_INTEGER pc;

    if ( is_first ) {

        static LARGE_INTEGER ifreq ;
        QueryPerformanceFrequency( &ifreq ) ;
        freq = ifreq.QuadPart;
        if ( freq < 1 ) {
            cerr << "my_gettimeofday(): no clock support"
                 << endl;
            return(-1);
        }

        is_first = false;

    }

    ::QueryPerformanceCounter(&pc);

    LONGLONG clicks = pc.QuadPart;
    lldiv_t dv = lldiv(clicks,freq);

    if ( dv.quot > LONG_MAX ) {
        cerr << "my_gettimeofday(): long int overflow!" << endl;
        return(-1);
    }
    tp->tv_sec = (long) (dv.quot);

    // I think the rem and freq will not lose precision when cast this way
    // I assume this because rem < freq ... and freq is not THAT big
    tp->tv_usec = (long) (1000000.0*((double)dv.rem/(double)freq));

    return 0;
}

#endif
