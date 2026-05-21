
#include "trick/Executive.hh"

#include <iostream>
#include <sstream>

int Trick::Executive::register_scheduler(Trick::Scheduler* scheduler)
{
    other_schedulers.push_back(scheduler);
    return (0);
}
