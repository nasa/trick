/*
    PURPOSE:
        (JSONVariableServer)
*/

#ifndef JSONVARIABLESERVER_HH
#define JSONVARIABLESERVER_HH

#include <string>
#include <iostream>
#include "trick/tc.h"
#include "trick/ThreadBase.hh"

namespace Trick {

/**
  This class runs the variable server listen loop.
  @author Alex Lin
 */
    class JSONVariableServer : public Trick::ThreadBase {

        public:
            JSONVariableServer() ;
            virtual ~JSONVariableServer() ;

            bool enabled ;

            /**
             @brief return the hostname of the listen device
             @return the hostname
            */
            const char * get_hostname() ;

            /**
             @brief return the port of the listen device
             @return the port
            */
            unsigned short get_port() ;

            /**
             @brief sets the port number of the listen device
            */
            void set_port(unsigned short in_port) ;

            /**
             @brief sets the source address for the listen device
            */
            void set_source_address(const char * address) ;

            /**
             @brief returns the source address of the listen device
            */
            std::string get_source_address() ;

            /**
             @brief default_data job that initially starts the listen device
            */
            virtual int init_listen_device() ;

            /**
             @brief init job that moves the listen device to the user specified
              source address and port and starts the listen thread.
            */
            virtual int init() ;

            /**
             @brief moves the listen device the the user specified source address
              and port.
            */
            virtual int check_and_move_listen_device() ;

            /**
             @brief thread body that listens for connections
            */
            virtual void * thread_body() ;

            /**
             @brief restart job restarts the listen device
            */
            int restart() ;

            /**
             @brief dump some debug information
            */
            virtual void dump( std::ostream & oss = std::cout ) ;

        protected:
            /** Requested variable server source address\n */
            std::string source_address ;       /**<  trick_units(--) */

            /** Requested variable server port number.\n */
            unsigned short port ;       /**<  trick_units(--) */

            /** User requested specific port number\n */
            bool user_port_requested ;  /**<  trick_units(--) */

            /** The listen device\n */
            TCDevice listen_dev;        /**<  trick_io(**) */

    } ;

}

#endif

