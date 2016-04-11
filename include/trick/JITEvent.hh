/**
PURPOSE:
    (blah.)
LIBRARY DEPENDENCY:
    ((../src/JITEvent.cpp))
*/

#ifndef JITEVENT_HH
#define JITEVENT_HH

#include <functional>
#include <iostream>
#include <string>

#include "trick/mm_macros.hh"
#include "trick/Event.hh"

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
        JITEvent() ;

        JITEvent(std::string func_name, std::string in_name = "JIT_no_name" , double in_cycle = 1.0) ;

        /** calls the function_ptr job */
        virtual int process( long long curr_time ) ;

        /** called when the event is added to the event manager */
        virtual void add() {} ;

        /** called when the event is removed from the event manager */
        virtual void remove() {} ;

        virtual void restart() ;

        std::string func_name ;

    protected:

        void get_func_ptr_from_name() ;

        /** pointer to funtion to run when event fires */
        int (*func_ptr)(void) ; // trick_io(**)

} ;

}

#endif
