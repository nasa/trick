#include "utils.h"

static int _idxAtTimeBinarySearch (double* timestamps,
                                  int low, int high, double time);

long round_10(long a)
{
    long r = a;

    if ( a % 10 != 0 ) {
        for ( int ii = 1; ii < 100; ++ii) {
            r = a + ii;
            if ( r % 10 == 0 ) break;
            r = a - ii;
            if ( r % 10 == 0 ) break;
        }
    }

    return r;
}

int getIndexAtTime( int ntimestamps, double* timestamps, double time)
{
        return _idxAtTimeBinarySearch( timestamps,
                                       0, ntimestamps-1, time );
}

int _idxAtTimeBinarySearch (double* timestamps,
                            int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                return ( timestamps[high] > time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (time == timestamps[mid]) {
                        return mid;
                } else if ( time < timestamps[mid] ) {
                        return _idxAtTimeBinarySearch(timestamps,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(timestamps,
                                                      mid+1, high, time);
                }
        }
}

