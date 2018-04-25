#ifndef MESSAGETCDEVICE_HH
#define MESSAGETCDEVICE_HH
/*
    PURPOSE: ( MessageTCDevice Class )
*/
#include <pthread.h>
#include <vector>

#include "trick/MessageSubscriber.hh"
#include "trick/ThreadBase.hh"
#include "trick/tc.h"

namespace Trick {

    class MessageTCDevice ;

    class MessageTCDeviceListenThread : public Trick::ThreadBase {

        public:
            MessageTCDeviceListenThread(MessageTCDevice * in_mtcd) ;
            virtual ~MessageTCDeviceListenThread() ;

            int get_port() ;

            int init_listen_device() ;
            int restart() ;

            virtual void * thread_body() ;
            virtual void dump( std::ostream & oss = std::cout ) ;

        protected:

            /** Pointer back to MessageTCDevice\n */
            MessageTCDevice * mtcd ;          /**< trick_io(**) */

            /** No handshake message server listen device.\n */
            TCDevice listen_dev ;             /**< trick_io(**) trick_units(--) */

    } ;


    /**
     * This MessageTCDevice is a class that inherits from MessageSubscriber.
     * It defines a type of MessageSubscriber that sends the messages to a receiver
     * through TCDevice when a proper socket connection is established.
     * Basically, it has a message server listen TCDevice that receives all the message
     * updates from a message publisher which it subscribes to and then writes the message
     * updates to all the connections it has if possible.
     */
    class MessageTCDevice : public MessageSubscriber {

        public:

            /**
             @brief The constructor.
             */
            MessageTCDevice();

            /**
             @brief The destructor.
             */
            ~MessageTCDevice();

            /**
             @brief Gets the listen thread.
             */
            Trick::MessageTCDeviceListenThread & get_listen_thread() ;

            /**
             @brief Adds a new connection.  Called from MessageTCDeviceListenThread
             */
            void add_connection( TCDevice * new_conn ) ;

            /**
             @brief Send the header & message to all open connections.
             */
            virtual void update( unsigned int level , std::string header, std::string message );

            /**
             @brief Initializes this subscriber.
             @return always 0
             */
            int default_data() ;

            /**
             @brief Initializes this subscriber.
             @return always 0
             */
            virtual int init() ;

            /**
             @brief Restarts this subscriber.
             @return always 0
             */
            int restart() ;

            /**
             @brief Shuts down this subscriber.
             @return always 0
             */
            virtual int shutdown() ;

            /** The port number for message socket connection. Copied out from listen_thread.\n */
            int port ;             /**< trick_units(--) */

        private:

            /** Thread to start listen device\n */
            MessageTCDeviceListenThread listen_thread ;

            /** Message buffer.\n */
            char * combined_message ;         /**< trick_io(**) trick_units(--) */

            /** List of open connections.\n */
            std::vector< TCDevice *> connections ; /**< trick_io(**) trick_units(--) */

    } ;

}

#endif

