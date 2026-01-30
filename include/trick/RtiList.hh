/*
    PURPOSE:
        (Real Time Injector)
*/

#ifndef RTILIST_HH
#define RTILIST_HH

#include "trick/RtiEvent.hh"
#include <vector>

namespace Trick
{

class RtiList
{
public:
    void execute(bool debug);

    void print_list();

    std::vector<RtiEventBase *> event_list;
};

} // namespace Trick

#endif
