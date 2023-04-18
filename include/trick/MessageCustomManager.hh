/*
    PURPOSE:
        (Track custom log files.)
    ASSUMPTIONS AND LIMITATIONS:
        (())
    PROGRAMMERS:
        (((Jackie Deans) (CACI) (4/23) (--) ())
*/

#ifndef MESSAGE_CUSTOM_MANAGER_HH
#define MESSAGE_CUSTOM_MANAGER_HH

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "trick/MessageCustomFile.hh"
#include "trick/MessageSubscriber.hh"


namespace Trick {

    /**
     * MessageCustomFile writes all messages its level to the given file
     */
    class MessageCustomManager : public MessageSubscriber {

        public:

            /**
             @brief The constructor.
             */
            MessageCustomManager() ;
            virtual ~MessageCustomManager() ;


            virtual int open_custom_message_file(std::string file_name, std::string subscriber_name, int level = -1);

            /**
             @brief Output message to the file.
             */
            virtual void update( unsigned int level , std::string header , std::string message );

            int restart();

        protected:
            std::vector<MessageCustomFile *> custom_message_files;
    } ;

}

#endif

