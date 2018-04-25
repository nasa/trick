/*
    PURPOSE:
        (Print messages to std::cout on a seperate thread)
*/

#ifndef MESSAGETHREADEDCOUT_HH
#define MESSAGETHREADEDCOUT_HH

#include <iostream>
#include "trick/ThreadBase.hh"
#include "trick/MessageSubscriber.hh"

namespace Trick {

	/**
	 * This MessageThreadedCout is a class that inherits from MessageSubscriber.
	 * It defines a type of MessageSubscriber with its received message sending to the standard output stream.
	 */
    class MessageThreadedCout : public MessageSubscriber , public Trick::ThreadBase {

        public:

            /**
             @brief Maximum number of items to hold in ring buffer
             */
            unsigned int max_buffer_items ; // trick_units(--)

            /**
             @brief Maximum size of string to hold
             */
            unsigned int max_buffer_size ; // trick_units(--)

            std::string color_code ;

            /**
             @brief The constructor.
             */
            MessageThreadedCout() ;
            virtual ~MessageThreadedCout() {} ;

            // From MessageSubscriber
            virtual int init() ;
            virtual void update( unsigned int level , std::string header , std::string message ) ;
            virtual int shutdown() ;

            // From Trick::ThreadBase
            virtual void * thread_body() ;
            virtual void dump( std::ostream & oss = std::cout ) ;

            // Specific Classes
            void write_pending_messages() ;

        protected:
            // After sim shutdown print immediately.
            bool print_immediate ;

            struct StringNode {
                std::string buffer ;
                size_t max_len ;
                StringNode * next ;
                StringNode(size_t str_len) : max_len(str_len) { buffer.reserve(max_len) ; } ;
                void copy( std::string &header, std::string & in_color_code, std::string & message ) {
                    buffer.assign(header, 0, max_len) ;
                    buffer.append(in_color_code, 0, max_len - buffer.length()) ;
                    buffer.append(message, 0, max_len - buffer.length()) ;
                    buffer.append("\033[00m", 0, max_len - buffer.length()) ;
                } ;
            } ;

            StringNode * copy_ptr ; /* trick_io(**) pointer to next buffer to copy data */
            StringNode * write_ptr ; /* trick_io(**) pointer to next buffer to write to screen */
            pthread_mutex_t write_mutex ; /* trick_io(**) mutex for writing */
    } ;

}

#endif

