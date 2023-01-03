/*
PURPOSE:      (Allows clients to get and set Trick parameters)
PROGRAMMERS: (((Alex Lin) (NASA) (8/06) (--)))
*/

#include <iostream>
#include <sstream>
#include <pthread.h>
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


int Trick::VariableServerSession::write_binary_data(const std::vector<VariableReference *>& given_vars, std::ostream& message_stream, VS_MESSAGE_TYPE message_type) {
    // int vars = 0;
    // int i;
    // int ret ;
    // int HeaderSize, MessageSize;
    // int NumVariablesProcessed;
    // unsigned int msg_type , offset, len ;
    // unsigned int size ;
    // unsigned int swap_int ;
    // char * address = 0 ;
    // char* param_name;

    // /* start the offset 4 bytes into the message, we'll subtract the sizeof offset at the end */
    // offset = sizeof(msg_type) + sizeof(offset) ;

    // if (byteswap) {
    //     /* Swap message type bytes */
    //     msg_type = trick_byteswap_int((int)message_type) ;
    // } else {
    //     msg_type = message_type;
    // }
    // memcpy(buf1, &msg_type , sizeof(msg_type)) ;
    // HeaderSize = sizeof(msg_type);

    // offset += sizeof(unsigned int) ;
    // HeaderSize += sizeof(unsigned int);

    // for (i = Start; i < (int)given_vars.size() ; i++) {

    //     // data to send was copied to buffer in copy_sim_data
    //     address = (char *)given_vars[i]->buffer_out;
    //     size = given_vars[i]->size ;

    //     param_name = given_vars[i]->ref->reference;
    //     len = strlen(param_name)  ;
    //     // when var_binary_nonames, do not put the variable names into the message to be sent
    //     if (binary_data_nonames) {
    //         MessageSize = sizeof(int) + sizeof(size) + size ;
    //     } else {
    //         MessageSize = sizeof(len) + len + sizeof(int) + sizeof(size) + size ;
    //     }

    //     /* make sure this message will fit in a packet by itself */
    //     if ( (HeaderSize + MessageSize) > MAX_MSG_LEN ) {
    //         message_publish(MSG_WARNING, "%p Variable Server buffer[%d] too small (need %d) for symbol %s, SKIPPING IT.\n",
    //                         &connection, MAX_MSG_LEN,
    //                         (int)(HeaderSize + MessageSize),
    //                         given_vars[i]->ref->reference );
    //         continue;
    //     }

    //     if ( (offset + MessageSize) < MAX_MSG_LEN ) {
    //         if (byteswap) {
    //             if (!binary_data_nonames) {
    //                 swap_int = trick_byteswap_int((int)len) ;
    //                 memcpy(&buf1[offset] , &swap_int , sizeof(len)) ;
    //                 offset += sizeof(len) ;

    //                 memcpy(&buf1[offset] , param_name , (size_t)len) ;
    //                 offset += len ;
    //             }

    //             swap_int = trick_byteswap_int(given_vars[i]->ref->attr->type) ;
    //             memcpy(&buf1[offset] , &swap_int , sizeof(int)) ;
    //             offset += sizeof(int) ;

    //             swap_int = trick_byteswap_int((int)size) ;
    //             memcpy(&buf1[offset] , &swap_int , sizeof(size)) ;
    //             offset += sizeof(size) ;

    //             /* TODO: There is a bug here, this call will want to swap the entire buffer, we may not have the whole buffer */
    //             trick_bswap_buffer(&buf1[offset], address, given_vars[i]->ref->attr, 1);
    //             offset += size ;
    //         }
    //         else {
    //             int temp_i ;
    //             unsigned int temp_ui ;

    //             if (!binary_data_nonames) {
    //                 memcpy(&buf1[offset] , &len , sizeof(len)) ;
    //                 offset += sizeof(len) ;

    //                 memcpy(&buf1[offset] , param_name , (size_t)len) ;
    //                 offset += len ;
    //             }

    //             memcpy(&buf1[offset] , &given_vars[i]->ref->attr->type , sizeof(int)) ;
    //             offset += sizeof(int) ;

    //             memcpy(&buf1[offset] , &size , sizeof(size)) ;
    //             offset += sizeof(size) ;

    //             switch ( given_vars[i]->ref->attr->type ) {
    //                 case TRICK_BITFIELD:
    //                     temp_i = GET_BITFIELD(address , given_vars[i]->ref->attr->size ,
    //                       given_vars[i]->ref->attr->index[0].start, given_vars[i]->ref->attr->index[0].size) ;
    //                     memcpy(&buf1[offset] , &temp_i , (size_t)size) ;
    //                 break ;
    //                 case TRICK_UNSIGNED_BITFIELD:
    //                     temp_ui = GET_UNSIGNED_BITFIELD(address , given_vars[i]->ref->attr->size ,
    //                             given_vars[i]->ref->attr->index[0].start, given_vars[i]->ref->attr->index[0].size) ;
    //                     memcpy(&buf1[offset] , &temp_ui , (size_t)size) ;
    //                 break ;
    //                 case TRICK_NUMBER_OF_TYPES:
    //                     // TRICK_NUMBER_OF_TYPES is an error case
    //                     temp_i = 0 ;
    //                     memcpy(&buf1[offset] , &temp_i , (size_t)size) ;
    //                 break ;
    //                 default:
    //                     memcpy(&buf1[offset] , address , (size_t)size) ;
    //                 break ;
    //             }
    //             offset += size ;
    //         }
    //     }
    //     else {
    //         /* indicate that we're over the maximum size */
    //         if (debug >= 2) {
    //             message_publish(MSG_DEBUG, "%p tag=<%s> var_server buffer[%d] too small (need %d), sending multiple binary packets.\n",
    //                     &connection, connection.client_tag, MAX_MSG_LEN,
    //                     (int)(offset + MessageSize) );
    //         }
    //         break ;
    //     }
    // }

    // /* adjust the header with the correct information reflecting what has been accomplished */
    // NumVariablesProcessed = i - Start;

    // offset -= sizeof(offset) ;
    // if (byteswap) {
    //     swap_int = trick_byteswap_int((int)offset) ;
    //     memcpy(buf1 + sizeof(msg_type) , &swap_int , sizeof(offset)) ;

    //     swap_int = trick_byteswap_int( NumVariablesProcessed ) ;
    //     memcpy( buf1 + sizeof(msg_type) + sizeof(offset), &swap_int , sizeof(swap_int)) ;
    // }
    // else {
    //     memcpy(buf1 + sizeof(msg_type) , &offset , sizeof(offset)) ;
    //     memcpy( buf1 + sizeof(msg_type) + sizeof(offset), &NumVariablesProcessed , sizeof( NumVariablesProcessed )) ;
    // }

    // if (debug >= 2) {
    //     message_publish(MSG_DEBUG, "%p tag=<%s> var_server sending %u binary bytes containing %d variables.\n", &connection,
    //             connection.client_tag, (unsigned int)(offset + sizeof(offset)), NumVariablesProcessed);
    // }

    // len = offset + sizeof(msg_type) ;
    // ret = tc_write(&connection, (char *) buf1, len);
    // if ( ret != (int)len ) {
    //     return(-1) ;
    // }

    /* return the index to the next symbol to send or V->num_vars if all done */
    return 0;
}

int Trick::VariableServerSession::write_ascii_data(const std::vector<VariableReference *>& given_vars, std::ostream& message_stream, VS_MESSAGE_TYPE message_type ) {

    // Load message type first
    message_stream << (int)message_type;

    // Load each variable
    for (int i = 0; i < given_vars.size(); i++) {
        message_stream << '\t';
        given_vars[i]->writeValueAscii(message_stream);
    }

    // End with newline
    message_stream << '\n';

    return 0;
}

int Trick::VariableServerSession::write_data() {
    return write_data(session_variables, VS_VAR_LIST);
}

int Trick::VariableServerSession::write_data(std::vector<VariableReference *>& given_vars, VS_MESSAGE_TYPE message_type) { 
    // do not send anything when there are no variables!
    if ( given_vars.size() == 0) {
        return(0);
    }

    if ( pthread_mutex_trylock(&copy_mutex) == 0 ) {
        // Check that all of the variables are staged
        for (VariableReference * variable : given_vars ) {
            if (!variable->isStaged()) {
                return 1;
            }
        }

        // Swap buffer_in and buffer_out for each vars[ii].
        for (VariableReference * variable : given_vars ) {
            variable->prepareForWrite();
        }

        pthread_mutex_unlock(&copy_mutex) ;

        std::stringstream message_stream;

        if (binary_data) {
            write_binary_data(given_vars, message_stream, message_type );
        } else {
            // ascii mode
            write_ascii_data(given_vars, message_stream, message_type );
        }

        // Write out the message
        std::string message = message_stream.str();
        return connection->write(message);
    }

    return 1;
}
