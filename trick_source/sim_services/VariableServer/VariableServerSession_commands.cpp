
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <udunits2.h>
#include "trick/VariableServerSession.hh"
#include "trick/variable_server_message_types.h"
#include "trick/memorymanager_c_intf.h"
// #include "trick/tc_proto.h"
#include "trick/exec_proto.h"
#include "trick/command_line_protos.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/TrickConstant.hh"
#include "trick/sie_c_intf.h"
#include "trick/UdUnits.hh"
#include "trick/map_trick_units_to_udunits.hh"


int Trick::VariableServerSession::var_add(std::string in_name) {
    VariableReference * new_var;
    if (in_name == "time") {
        new_var = new VariableReference(in_name, &time);
    } else {
        new_var = new VariableReference(in_name);
    }

    session_variables.push_back(new_var) ;

    return(0) ;
}

int Trick::VariableServerSession::var_add(std::string var_name, std::string units_name) {
    var_add(var_name) ;
    var_units(var_name, units_name) ;
    return(0) ;
}

// Helper function for var_send_once
std::vector<std::string> split (const std::string& str, const char delim) {
    std::stringstream ss(str);
    std::string s;
    std::vector<std::string> ret;
    while (std::getline(ss, s, delim)) {
        ret.push_back(s);
    }
    return ret;
}

int Trick::VariableServerSession::var_send_once(std::string in_name, int num_vars) {
    std::vector<std::string> var_names = split(in_name, ',');

    if (var_names.size() != num_vars) {
        message_publish(MSG_ERROR, "Number of variables sent to var_send_once (%d) does not match num_vars (%d).\n", var_names.size(), num_vars);
        return -1;
    }

    std::vector<VariableReference *> given_vars;
    for (auto& varName : var_names) {
        VariableReference * new_var;
        if (varName == "time") {
            new_var = new VariableReference(varName, &time);
        } else {
            new_var = new VariableReference(varName);
        }
        given_vars.push_back(new_var);
    }
    copy_sim_data(given_vars, false);
    write_data(given_vars, VS_SEND_ONCE);

    return(0) ;
}


int Trick::VariableServerSession::var_remove(std::string in_name) {

    for (unsigned int ii = 0 ; ii < session_variables.size() ; ii++ ) {
        std::string var_name = session_variables[ii]->getName();
        if ( ! var_name.compare(in_name) ) {
            delete session_variables[ii];
            session_variables.erase(session_variables.begin() + ii) ;
            break ;
        }
    }

    return(0) ;

}

int Trick::VariableServerSession::var_units(std::string var_name, std::string units_name) {    
    VariableReference * variable = find_session_variable(var_name);

    if (variable == NULL) {
        // TODO: ERROR LOGGER HERE
        return -1;
    }

    return variable->setRequestedUnits(units_name);
}

int Trick::VariableServerSession::var_exists(std::string in_name) {
    char buf1[5] ;
    bool error = false ;

    unsigned int msg_type ;
    REF2* var_ref = ref_attributes(in_name.c_str());

    if ( var_ref == (REF2*)NULL ) {
        error = true;
    }

    if (binary_data) {
        /* send binary 1 or 0 */
        msg_type = VS_VAR_EXISTS ;
        memcpy(buf1, &msg_type , sizeof(msg_type)) ;

        buf1[4] = (error==false);

        if (debug >= 2) {
            // message_publish(MSG_DEBUG, "%p tag=<%s> var_server sending 1 binary byte\n", &connection, connection.client_tag);
        }

        connection->write(buf1, 5);
    } else {
        /* send ascii "1" or "0" */
        sprintf(buf1, "%d\t%d\n", VS_VAR_EXISTS, (error==false));
        if (debug >= 2) {
            // message_publish(MSG_DEBUG, "%p tag=<%s> var_server sending:\n%s\n", &connection, connection.client_tag, buf1) ;
        }
        std::string write_string(buf1);
        if (write_string.length() != strlen(buf1)) {
            std::cout << "PROBLEM WITH STRING LENGTH: VAR_EXISTS ASCII" << std::endl;
        }
        connection->write(write_string);
    }

    return(0) ;
}

int Trick::VariableServerSession::var_clear() {

    while( !session_variables.empty() ) {
        delete session_variables.back();
        session_variables.pop_back();
    }

    return(0) ;
}

int Trick::VariableServerSession::var_send() {
    copy_sim_data();
    write_data();
    return(0) ;
}

int Trick::VariableServerSession::var_cycle(double in_rate) {
    update_rate = in_rate ;
    cycle_tics = (long long)(update_rate * exec_get_time_tic_value()) ;
    return(0) ;
}

bool Trick::VariableServerSession::get_pause() {
    return pause_cmd ;
}

void Trick::VariableServerSession::set_pause( bool on_off) {
    pause_cmd = on_off ;
}

int Trick::VariableServerSession::var_exit() {
    exit_cmd = true ;
    return(0) ;
}

int Trick::VariableServerSession::var_validate_address(bool on_off) {
    validate_address = on_off ;
    return(0) ;
}

int Trick::VariableServerSession::var_debug(int level) {
    debug = level ;
    return(0) ;
}

int Trick::VariableServerSession::var_ascii() {
    binary_data = 0 ;
    return(0) ;
}

int Trick::VariableServerSession::var_binary() {
    binary_data = 1 ;
    return(0) ;
}

int Trick::VariableServerSession::var_binary_nonames() {
    binary_data = 1 ;
    binary_data_nonames = 1 ;
    return(0) ;
}

int Trick::VariableServerSession::var_set_copy_mode(int mode) {
    if ( mode >= VS_COPY_ASYNC and mode <= VS_COPY_TOP_OF_FRAME ) {
        copy_mode = (VS_COPY_MODE)mode ;
        if ( copy_mode == VS_COPY_SCHEDULED ) {
            long long sim_time_tics ;
            sim_time_tics = exec_get_time_tics() ;
            // round the next call time to a multiple of the cycle
            sim_time_tics -= sim_time_tics % cycle_tics ;
            next_tics = sim_time_tics + cycle_tics ;

            sim_time_tics = exec_get_freeze_time_tics() ;
            // round the next call time to a multiple of the cycle
            sim_time_tics -= sim_time_tics % cycle_tics ;
            freeze_next_tics = sim_time_tics + cycle_tics ;

        } else {
            next_tics = TRICK_MAX_LONG_LONG ;
        }
        return 0 ;
    }
    return -1 ;
}

int Trick::VariableServerSession::var_set_write_mode(int mode) {
    if ( mode >= VS_WRITE_ASYNC and mode <= VS_WRITE_WHEN_COPIED ) {
        write_mode = (VS_WRITE_MODE)mode ;
        return 0 ;
    }
    return -1 ;
}

int Trick::VariableServerSession::var_sync(int mode) {

    switch (mode) {
        case 1:
            var_set_copy_mode(VS_COPY_SCHEDULED) ;
            var_set_write_mode(VS_WRITE_ASYNC) ;
            break ;
        case 2:
            var_set_copy_mode(VS_COPY_SCHEDULED) ;
            var_set_write_mode(VS_WRITE_WHEN_COPIED) ;
            break ;
        case 0:
        default:
            var_set_copy_mode(VS_COPY_ASYNC) ;
            var_set_write_mode(VS_WRITE_ASYNC) ;
            break ;
    }

    return 0 ;
}

int Trick::VariableServerSession::var_set_frame_multiple(unsigned int mult) {
    frame_multiple = mult ;
    return 0 ;
}

int Trick::VariableServerSession::var_set_frame_offset(unsigned int offset) {
    frame_offset = offset ;
    return 0 ;
}

int Trick::VariableServerSession::var_set_freeze_frame_multiple(unsigned int mult) {
    freeze_frame_multiple = mult ;
    return 0 ;
}

int Trick::VariableServerSession::var_set_freeze_frame_offset(unsigned int offset) {
    freeze_frame_offset = offset ;
    return 0 ;
}

int Trick::VariableServerSession::var_byteswap(bool on_off) {
    byteswap = on_off ;
    return(0) ;
}

bool Trick::VariableServerSession::get_send_stdio() {
    return send_stdio ;
}

int Trick::VariableServerSession::set_send_stdio(bool on_off) {
    send_stdio = on_off ;
    return(0) ;
}

int Trick::VariableServerSession::send_list_size() {
    
    unsigned int msg_type = VS_LIST_SIZE;
    int var_count = session_variables.size();

    // send number of variables
    if (binary_data) {
        // send in the binary message header format:
        // <message_indicator><message_size><number_of_variables>
        char buf1[12] ;

        unsigned int msg_type = VS_LIST_SIZE;
        memcpy(buf1, &msg_type , sizeof(msg_type)) ;

        memset(&(buf1[4]), 0, sizeof(int)); // message size = 0
        memcpy(&(buf1[8]), &var_count, sizeof(var_count));

        if (debug >= 2) {
            message_publish(MSG_DEBUG, "%p tag=<%s> var_server sending %d event variables\n", connection, connection->get_client_tag().c_str(), var_count);
        }

        connection->write(buf1, sizeof (buf1));
    } else {
        std::stringstream write_string;
        write_string << VS_LIST_SIZE << "\t" << var_count << "\n";
        // ascii
        if (debug >= 2) {
            message_publish(MSG_DEBUG, "%p tag=<%s> var_server sending number of event variables:\n%s\n", connection, connection->get_client_tag().c_str(), write_string.str().c_str()) ;
        }

        connection->write(write_string.str());
    }

    return 0 ;
}

int Trick::VariableServerSession::transmit_file(std::string sie_file) {
    const unsigned int packet_size = 4095 ;
    FILE * fp ;
    unsigned int file_size ;
    unsigned int current_size = 0 ;
    unsigned int bytes_read ;
    char buffer[packet_size+1] ;
    int ret ;

    if (debug >= 2) {
        message_publish(MSG_DEBUG,"%p tag=<%s> var_server opening %s.\n", connection, connection->get_client_tag().c_str(), sie_file.c_str()) ;
    }

    if ((fp = fopen(sie_file.c_str() , "r")) == NULL ) {
        message_publish(MSG_ERROR,"Variable Server Error: Cannot open %s.\n", sie_file.c_str()) ;
        sprintf(buffer, "%d\t-1\n", VS_SIE_RESOURCE) ;
        std::string message(buffer);
        connection->write(message);
        return(-1) ;
    }

    fseek(fp , 0L, SEEK_END) ;
    file_size = ftell(fp) ;

    sprintf(buffer, "%d\t%u\n\0" , VS_SIE_RESOURCE, file_size) ;
    std::string message(buffer);
    connection->write(message);
    rewind(fp) ;

    // Switch to blocking writes since this could be a large transfer.
    if (connection->setBlockMode(TC_COMM_BLOCKIO)) {
        message_publish(MSG_DEBUG,"Variable Server Error: Failed to set TCDevice to TC_COMM_BLOCKIO.\n");
    }

    while ( current_size < file_size ) {
        bytes_read = fread(buffer , 1 , packet_size , fp) ;
        message = std::string(buffer);
        message.resize(bytes_read);
        ret = connection->write(message);
        if (ret != (int)bytes_read) {
            message_publish(MSG_ERROR,"Variable Server Error: Failed to send SIE file. Bytes read: %d Bytes sent: %d\n", bytes_read, ret) ;
            return(-1);
        }
        current_size += bytes_read ;
    }

    // Switch back to non-blocking writes.
    if (connection->setBlockMode(TC_COMM_NOBLOCKIO)) {
        message_publish(MSG_ERROR,"Variable Server Error: Failed to set TCDevice to TC_COMM_NOBLOCKIO.\n");
        return(-1);
    }

    return(0) ;
}

int Trick::VariableServerSession::send_file(std::string file_name) {
    return transmit_file(file_name) ;
}

int Trick::VariableServerSession::send_sie_resource() {
    sie_append_runtime_objs() ;
    return transmit_file(std::string(command_line_args_get_default_dir()) + "/S_sie.resource") ;
}

int Trick::VariableServerSession::send_sie_class() {
    sie_class_attr_map_print_xml() ;
    return transmit_file(std::string(command_line_args_get_default_dir()) + "/" + "S_sie_class.xml") ;
}

int Trick::VariableServerSession::send_sie_enum() {
    sie_enum_attr_map_print_xml() ;
    return transmit_file(std::string(command_line_args_get_default_dir()) + "/" + "S_sie_enum.xml") ;
}

int Trick::VariableServerSession::send_sie_top_level_objects() {
    sie_top_level_objects_print_xml() ;
    return transmit_file(std::string(command_line_args_get_default_dir()) + "/" + "S_sie_top_level_objects.xml") ;
}
