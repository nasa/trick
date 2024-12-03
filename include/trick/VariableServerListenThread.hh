/*
    PURPOSE:
        (VariableServerListenThread)
*/

#ifndef VARIABLESERVERLISTENTHREAD_HH
#define VARIABLESERVERLISTENTHREAD_HH

#include <string>
#include <iostream>
#include "trick/TCPClientListener.hh"
#include "trick/SysThread.hh"
#include "trick/MulticastGroup.hh"


namespace Trick {

/**
  This class runs the variable server listen loop.
  @author Alex Lin
  @author Jackie Deans (2023)
 */
    class VariableServerListenThread : public Trick::SysThread {

        public:
            VariableServerListenThread() ;
            VariableServerListenThread(TCPClientListener * listener);

            virtual ~VariableServerListenThread() ;

            const char * get_hostname() ;

            unsigned short get_port() ;
            void set_port(unsigned short in_port) ;

            std::string get_user_tag() ;
            const std::string& get_user_tag_ref() ;
            void set_user_tag(std::string in_tag) ;

            void set_source_address(const char * address) ;
            std::string get_source_address() ;

            bool get_broadcast() ;
            void set_broadcast(bool in_broadcast) ;

            virtual int init_listen_device() ;
            virtual int check_and_move_listen_device() ;

            virtual void * thread_body() ;

            int restart() ;

            // pause and restart listen thread during checkpoint reload
            void pause_listening() ;
            void restart_listening() ;

            void set_multicast_group (MulticastGroup * group);

            virtual void dump( std::ostream & oss = std::cout ) ;

            void shutdownConnections();

        protected:
            void initializeMulticast();

            /** Requested variable server source address\n */
            std::string _requested_source_address ;       /**<  trick_units(--) */

            /** Requested variable server port number.\n */
            unsigned short _requested_port ;       /**<  trick_units(--) */

            /** User requested specific port number\n */
            bool _user_requested_address ;  /**<  trick_units(--) */

            /** User defined unique tag to easily identify this variable server port.\n */
            std::string _user_tag;          /**<  trick_units(--) */

            /** Turn on/off broadcasting of variable server port.\n */
            bool _broadcast ;       /**<  trick_units(--) */

            /** The listen device */
            TCPClientListener * _listener;        /**<  trick_io(**) trick_units(--)  */

            /* Multicast broadcaster */
            MulticastGroup * _multicast;     /**<  trick_io(**) trick_units(--)  */

            bool allowConnections;                   /**<  trick_io(**) trick_units(--)  */
            unsigned int pendingConnections;         /**<  trick_io(**) trick_units(--)  */
            pthread_mutex_t connectionMutex;         /**<  trick_io(**) trick_units(--)  */
            pthread_cond_t  noPendingConnections_cv; /**<  trick_io(**) trick_units(--)  */

    } ;

}

#endif
