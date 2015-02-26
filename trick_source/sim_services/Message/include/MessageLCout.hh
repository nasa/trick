/* 
 * $Id: MessageLCout.hh 3383 2014-01-22 23:18:15Z alin $
 */

/**
 * @file MessageLCout.hh
 */

#ifndef _EXECMESSAGELCOUT_HH_
#define _EXECMESSAGELCOUT_HH_

#include <iostream>
#include <map>
#include <stdlib.h>
#include "sim_services/Message/include/MessageSubscriber.hh"

namespace Trick {

	/**
	 * This MessageLCout is a class that inherits from MessageSubscriber.
	 * It defines a type of MessageSubscriber with its received message sending to the standard output stream.
	 */
    class MessageLCout : public MessageSubscriber {

        public:
            /**
             @brief Constructor.
             */
            MessageLCout() ;

            /**
             @brief Output header & message to standard output stream.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;

        private:
            /** Map characters in message to Leet equivalent.\n */
            std::map<char,std::string> subs ;

            unsigned int seed ;

    } ;

}

#endif

