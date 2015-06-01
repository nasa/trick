/*
    PURPOSE:
        (Playback messages)
*/

#ifndef PLAYBACKFILE_HH
#define PLAYBACKFILE_HH

#include <string>
#include <iostream>
#include <fstream>
#include "trick/MessageFile.hh"

namespace Trick {

    /**
     * This PlaybackFile is a class that inherits from MessageFile.
     * It defines a type of MessageSubscriber with its received message sending to a file.
     */
    class PlaybackFile : public MessageFile {

        public:

            PlaybackFile() ;

            /**
             @brief Output message to the file.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;
    } ;

}

#endif

