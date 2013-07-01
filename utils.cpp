#include "utils.h"

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
