
#include <iostream>
#include <unistd.h>

#include "trick/MessageFile.hh"
#include "trick/command_line_protos.h"

/**
@details
-# Initializes everything
*/
Trick::MessageFile::MessageFile() {

    enabled = 1 ;
    color = 0 ;
    file_name = "send_hs" ;
    name = "file" ;

}

/**
@details
-# Overwrites the file name
*/
int Trick::MessageFile::set_file_name(std::string in_name) {

    file_name = in_name ;
    return(0) ;

}

/**
@details
-# Deletes the current output file
-# Opens a new file with the name "file_name"
*/
int Trick::MessageFile::init() {

    unlink((std::string(command_line_args_get_output_dir()) + "/" + file_name).c_str()) ;
    out_stream.open((std::string(command_line_args_get_output_dir()) + "/" + file_name).c_str() , std::fstream::out | std::fstream::app ) ;
    return(0) ;
}

/**
@details
-# If enabled and level < 100
    -# Write the header and message to the file stream
    -# Flush the stream
*/
void Trick::MessageFile::update( unsigned int level , std::string header, std::string message ) {

    if ( enabled && level < 100 ) {
        out_stream << header << message ;
        out_stream.flush() ;
    }

}

/**
@details
-# Close the file stream
*/
Trick::MessageFile::~MessageFile() {
    out_stream.close() ;
}

