
#include <iostream>
#include <unistd.h>

#include "sim_services/Message/include/PlaybackFile.hh"
#include "sim_services/Message/include/message_type.h"

Trick::PlaybackFile::PlaybackFile() {
    file_name = "varserver_log" ;
    name = "varserver_log" ;
}

void Trick::PlaybackFile::update( unsigned int level , std::string header __attribute__ ((unused)) , std::string message ) {

    if ( enabled && level == MSG_PLAYBACK ) {
        out_stream << message ;
        out_stream.flush() ;
    }

}

