#ifndef MESSAGEPUBLISHER_HH
#define MESSAGEPUBLISHER_HH
/*
    PURPOSE: ( MessagePublisher Class )
*/
#include <string>
#include <list>
#include "trick/MessageSubscriber.hh"

namespace Trick {

	/**
	 * This class provides the capability of publishing executive and/or model messages.
	 */
    class MessagePublisher {

        private:
            /** List of subscribers subscribed to this publisher.\n */
            std::list <MessageSubscriber *> subscribers ;    /**< trick_units(--) */

            /** Copied from the executive time_tic_value.\n */
            int tics_per_sec ;

            /** Number of significant digits to use in time print.\n */
            int num_digits ;

            /** Print format that accomodates enough significant digits to handle tics_per_sec */
            char print_format[64] ;

            /**
             @brief sets the print format
             */
            void set_print_format() ;

        public:

            /** Name of the simulation, usually inputted through the input processor (default is " ").\n */
            std::string sim_name;                            /**< trick_units(--) */

            /**
             @brief The constructor.
             */
            MessagePublisher() ;

            /**
             @brief Initialization job.  Sets tics_per_sec and print format.
             @ return 0
             */
            int init() ;

            /**
             @brief Add a message subscriber to this publisher's subscriber list, which will output published messages in some manner.
             @param in_ms - an instance of Trick::MessageSubscriber that wants to subscribe to this publisher.
             */
            void subscribe(MessageSubscriber *in_ms) { subscribers.push_back(in_ms) ; } ;

            /**
             @brief Remove a message subscriber from this publisher's subscriber list.
             @param in_ms - an instance of Trick::MessageSubscriber that needs unsubscribe from this publisher.
             */
            void unsubscribe(MessageSubscriber *in_ms) { subscribers.remove(in_ms) ; } ;

            /**
             @brief Publish a message with specified level and header.
             @param level - message level
             @param message - the text of the message
             @return always 0
             */
            int publish(int level, std::string message) ;

            /**
             @brief gets the subscriber from the list
             @param sub_name - name of the subscriber to get.
             */
            Trick::MessageSubscriber * getSubscriber(std::string sub_name) ;

    } ;

}

#endif

