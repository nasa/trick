/*
PURPOSE:      (Allows clients to get and set Trick parameters)
PROGRAMMERS: (((Alex Lin) (NASA) (8/06) (--)))
*/

#include <iostream>
#include <sstream>
#include <pthread.h>
#include <iomanip> // for setprecision
#include "trick/VariableServerSession.hh"
#include "trick/parameter_types.h"
#include "trick/bitfield_proto.h"
#include "trick/trick_byteswap.h"
#include "trick/tc_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"


extern "C" {
    void *trick_bswap_buffer(void *out, void *in, ATTRIBUTES * attr, int tofrom) ;
}

#define MAX_MSG_LEN    8192


int Trick::VariableServerSession::write_binary_data(const std::vector<VariableReference *>& given_vars, VS_MESSAGE_TYPE message_type) {
    // Some constants to make size calculations more readable
    static const int header_size = 12;
    static const int sizeof_size = 4;
    static const int type_size = 4;

    // Calculate total message size
    int total_size = header_size;
    for (const VariableReference * var : given_vars) {
        int total_var_size = 0;
        if (!binary_data_nonames) {
            total_var_size += sizeof_size;
            total_var_size += strlen(var->getName());
        }

        total_var_size += type_size;
        total_var_size += sizeof_size;
        total_var_size += var->getSize();

        // Todo: Check that total_var_size fits in max message length
        total_size += total_var_size;
    }
 
    std::stringstream stream;

    int written_message_type = message_type;
    int written_header_size = total_size - 4;
    int written_num_vars = given_vars.size();

    if (byteswap) {
        written_message_type = trick_byteswap_int(written_message_type);
        written_header_size = trick_byteswap_int(written_header_size);
        written_num_vars = trick_byteswap_int(written_num_vars);
    }

    // Write the header first
    stream.write((char *)(&written_message_type), sizeof(int)); 
    stream.write((char *)(&written_header_size), sizeof(int)); 
    stream.write ((char *)(&written_num_vars), sizeof(int));

    // Write variables next
    for (VariableReference * var : given_vars) {
        if (!binary_data_nonames) {
            var->writeNameBinary(stream, byteswap);
        }
        var->writeTypeBinary(stream, byteswap);
        var->writeSizeBinary(stream, byteswap);
        var->writeValueBinary(stream, byteswap);
    }

    char write_buf[MAX_MSG_LEN];
    stream.read(write_buf, total_size);
    connection->write(write_buf, total_size);

    return 0;
}

int Trick::VariableServerSession::write_ascii_data(const std::vector<VariableReference *>& given_vars, VS_MESSAGE_TYPE message_type ) {
    // Load message type first
    std::stringstream message_stream;
    message_stream << (int)message_type;

    // Load each variable
    for (int i = 0; i < given_vars.size(); i++) {

        std::stringstream var_stream;
        given_vars[i]->writeValueAscii(var_stream);

        // Check that there's enough room for the next variable, tab character, and possible newline
        if (message_stream.gcount() + var_stream.gcount() + 2 > MAX_MSG_LEN) {
            // Write out an incomplete message
            int result = connection->write(message_stream.str());
            if (result != 0)
                return result;
            // Clear out the message stream
            message_stream = std::stringstream("");
        }

        // Concatenate
        message_stream << "\t" << var_stream.rdbuf();
    }

    // End with newline
    message_stream << '\n';
    int result = connection->write(message_stream.str());
    return result;
}

int Trick::VariableServerSession::write_data() {
    return write_data(session_variables, VS_VAR_LIST);
}

int Trick::VariableServerSession::write_data(std::vector<VariableReference *>& given_vars, VS_MESSAGE_TYPE message_type) { 
    // do not send anything when there are no variables!
    if ( given_vars.size() == 0) {
        return(0);
    }

    int result = 0;

    if ( pthread_mutex_trylock(&copy_mutex) == 0 ) {
        // Check that all of the variables are staged
        for (VariableReference * variable : given_vars ) {
            if (!variable->isStaged()) {
                pthread_mutex_unlock(&copy_mutex) ;
                return 1;
            }
        }

        // Swap buffer_in and buffer_out for each vars[ii].
        for (VariableReference * variable : given_vars ) {
            variable->prepareForWrite();
        }

        pthread_mutex_unlock(&copy_mutex) ;

        // Send out in correct format
        if (binary_data) {
            result = write_binary_data(given_vars, message_type );
        } else {
            // ascii mode
            result = write_ascii_data(given_vars, message_type );
        }
    }

    return result;
}
