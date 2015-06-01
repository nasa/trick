
#ifndef EVENTMANAGER_C_INTF_HH
#define EVENTMANAGER_C_INTF_HH

#include "trick/Event.hh"

// Get an event by name
Trick::Event * event_manager_get_event( std::string in_name ) ;

// Add a cyclic event
int event_manager_add_event( Trick::Event * in_event ) ;

// Add an instrumented event
int event_manager_add_event_before( Trick::Event * in_event, std::string target_name, unsigned int target_inst = 1 ) ;
int event_manager_add_event_after( Trick::Event * in_event, std::string target_name, unsigned int target_inst = 1 ) ;

// Remove an event
int event_manager_remove_event( Trick::Event * in_event ) ;

#endif
