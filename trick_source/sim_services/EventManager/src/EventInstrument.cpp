
#include "sim_services/EventManager/include/EventInstrument.hh"
#include "sim_services/Executive/include/exec_proto.hh"

Trick::EventInstrument::EventInstrument(Trick::Event * in_event, Trick::JobData * in_target_job) :
 InstrumentBase(in_target_job),
 event(in_event) {}

// The instrument call processes the event.
int Trick::EventInstrument::call() {
    event->process(exec_get_time_tics()) ;
    return 0 ;
}

