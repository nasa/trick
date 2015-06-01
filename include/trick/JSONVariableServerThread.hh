/*
    PURPOSE:
        (JSONVariableServerThread)
*/

#ifndef JSONVARIABLESERVERTHREAD_HH
#define JSONVARIABLESERVERTHREAD_HH

#include <pthread.h>
#include "trick/tc.h"
#include "trick/ThreadBase.hh"

namespace Trick {

/**
  This class provides variable server command processing on a separate thread for each client.
  @author Alex Lin
 */
    class JSONVariableServerThread : public Trick::ThreadBase {

        public:
            /**
             @brief Constructor.
             @param listen_dev - the TCDevice set up in listen()
            */
            JSONVariableServerThread(TCDevice * in_listen_dev ) ;

            virtual ~JSONVariableServerThread() ;

            /**
             @brief The main loop of the variable server thread that reads and processes client commands.
             @return always 0
            */
            virtual void * thread_body() ;

            /**
             @brief parses the incoming request and returns the result.
            */
            void parse_request() ;

        protected:

            /** The trickcomm device used for the connection to the client.\n */
            TCDevice connection ;            /**<  trick_io(**) */

            /** Maximum size of incoming message\n */
            static const unsigned int MAX_CMD_LEN = 200000 ;

            /** hostname for connection from listen device\n */
            std::string hostname ;           /**< trick_io(**) */

            /** port for connection from listen device\n */
            unsigned short  port ;           /**< trick_io(**) */

            /** The command area\n */
            char * incoming_msg ;            /**< trick_io(**) */

            /** Return the top page */
            int get_top_page(std::stringstream & body) ;

            /** Process all pages starting with /commands */
            int get_commands(std::stringstream & body, char * path ) ;

            /** Process all pages starting with /vars */
            int get_vars(std::stringstream & body, char * path ) ;
    } ;
}

#endif

