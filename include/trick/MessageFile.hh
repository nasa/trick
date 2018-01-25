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

#ifndef MESSAGEFILE_HH
#define MESSAGEFILE_HH

#include <string>
#include <iostream>
#include <fstream>
#include "trick/MessageSubscriber.hh"

namespace Trick {

    /**
     * This MessageFile is a class that inherits from MessageSubscriber.
     * It defines a type of MessageSubscriber with its received message sending to a file.
     */
    class MessageFile : public MessageSubscriber {

        public:

            /** The file name of a file which the messages goes to. \n*/
            std::string file_name ; /**< trick_units(--) trick_io(*i) */

            /**
             @brief The constructor.
             */
            MessageFile() ;

            /**
             @brief The destructor.
             */
            ~MessageFile() ;

            /**
             @brief Output message to the file.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;

            /**
             @brief Set a file name for a file which the messages received by this subscriber goes to.
             @return always 0
             */
            int set_file_name(std::string in_name) ;

            /**
             @brief Initializes this subscriber.
             @return always 0
             */
            virtual int init() ;

        protected:
            /** The output file stream. \n */
            std::fstream out_stream ;    /**< trick_io(**) */

            // This object is not copyable (fstream).  Add private copy so SWIG knows not to wrap this
            void operator =(const Trick::MessageFile &) {};

    } ;

}

#endif

