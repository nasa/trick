/*************************************************************************
PURPOSE: (Example of "automatic" job, i.e., a self scheduling job.)
LIBRARY DEPENDENCIES:
    ((fibonacci_time/src/Fibonacci_time.o))
**************************************************************************/
#ifndef _fibonacci_time_hh_
#define _fibonacci_time_hh_

class Fibonacci_time
{
public:
    int N;
    int tics_per_second;

    int default_data();
    int state_init();
    int scheduled_job_1();
    int automatic_job_1();
};
#endif
