#ifndef MESSAGELCOUT_HH
#define MESSAGELCOUT_HH
/*
    PURPOSE: ( MessageLCout Class )
*/
#include <iostream>
#include <map>
#include <stdlib.h>
#include "trick/MessageSubscriber.hh"

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

