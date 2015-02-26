/**
PURPOSE:
    (blah.)
LIBRARY DEPENDENCY:
    ((../src/JITEvent.cpp))
*/

#ifndef _JITCYCLICEVENT_HH_
#define _JITCYCLICEVENT_HH_

#include <functional>
#include <iostream>
#include <string>

#include "sim_services/EventManager/include/Event.hh"

namespace Trick {

/**
 * This is a cyclic event class.  It holds a cycle time, and the function to call.
 *
 * @author Alexander S. Lin
 *
 * @date April 2014
 *
 */
class JITEvent : public Trick::Event {

    public:
        JITEvent(int(*in_func_ptr)(void), std::string in_name = "JIT_no_name" , double in_cycle = 1.0) ;

        /** calls the function_ptr job */
        virtual int process( long long curr_time ) ;

        /** called when the event is added to the event manager */
        virtual void add() {} ;

        /** called when the event is removed from the event manager */
        virtual void remove() {} ;

        virtual void restart() {} ;


    protected:

        /** pointer to funtion to run when event fires */
        int (*func_ptr)(void) ;

} ;

}

#endif
