/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.))
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef MESSAGECOUT_HH
#define MESSAGECOUT_HH

#include <iostream>
#include "trick/MessageSubscriber.hh"

namespace Trick {

	/**
	 * This MessageCout is a class that inherits from MessageSubscriber.
	 * It defines a type of MessageSubscriber with its received message sending to the standard output stream.
	 */
    class MessageCout : public MessageSubscriber {

        public:

            /**
             @brief The constructor.
             */
            MessageCout() ;

            /**
             @brief Output header & message to standard output stream.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;

    } ;

}

#endif

