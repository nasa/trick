
#include <iostream>

#include "trick/VariableServer.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/map_trick_units_to_udunits.hh"

extern Trick::VariableServer * the_vs ;

Trick::VariableServerThread * get_vst() {
    return the_vs->get_vst(pthread_self()) ;
}

int var_add(std::string in_name) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_add(in_name) ;
    }
    return(0) ;
}

int var_add(std::string in_name, std::string in_units) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_add(in_name, in_units) ;
    }
    return(0) ;
}

int var_remove(std::string in_name) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_remove(in_name) ;
    }
    return(0) ;
}

int var_units(std::string var_name , std::string units_name) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_units(var_name , units_name) ;
    }
    return(0) ;
}

int var_exists(std::string in_name) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_exists(in_name) ;
    }
    return(0) ;
}

int var_send() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_send() ;
    }
    return(0) ;
}

int var_clear() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_clear() ;
    }
    return(0) ;
}

int var_cycle(double in_rate) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_cycle(in_rate) ;
    }
    return(0) ;
}

int var_pause() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->set_pause(true) ;
    }
    return(0) ;

}

int var_unpause() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->set_pause(false) ;
    }
    return(0) ;

}

int var_exit() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_exit() ;
    }
    return(0) ;
}

int var_validate_address(int on_off) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_validate_address((bool)on_off) ;
    }
    return(0) ;
}

int var_debug(int level) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_debug(level) ;
    }
    return(0) ;
}

int var_ascii() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_ascii() ;
    }
    return(0) ;
}

int var_binary() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_binary() ;
    }
    return(0) ;
}

int var_retry_bad_ref() {
    message_publish(MSG_WARNING,"var_retry_bad_ref has been deprecated\n") ;
    return(0) ;
}

int var_binary_nonames() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_binary_nonames() ;
    }
    return(0) ;
}

int var_set_copy_mode(int mode) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_copy_mode(mode) ;
        if ( mode == VS_COPY_SCHEDULED ) {
            the_vs->get_next_sync_call_time() ;
            the_vs->get_next_freeze_call_time() ;
        }
    }
    return 0 ;
}

int var_set_write_mode(int mode) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_write_mode(mode) ;
    }
    return 0 ;
}

int var_set_send_stdio(int mode) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->set_send_stdio((bool)mode) ;
    }
    return 0 ;
}

int var_sync(int mode) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_sync(mode) ;
        if ( mode ) {
            the_vs->get_next_sync_call_time() ;
            the_vs->get_next_freeze_call_time() ;
        }
    }
    return(0) ;
}

int var_set_frame_multiple(unsigned int mult) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_frame_multiple(mult) ;
    }
    return 0 ;
}

int var_set_frame_offset(unsigned int offset) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_frame_offset(offset) ;
    }
    return 0 ;
}

int var_set_freeze_frame_multiple(unsigned int mult) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_freeze_frame_multiple(mult) ;
    }
    return 0 ;
}

int var_set_freeze_frame_offset(unsigned int offset) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_set_freeze_frame_offset(offset) ;
    }
    return 0 ;
}

int var_byteswap(bool on_off) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_byteswap(on_off) ;
    }
    return(0) ;
}

int var_signal() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_signal() ;
    }
    return(0) ;
}

int var_multicast(bool on_off) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->var_multicast(on_off) ;
    }
    return(0) ;
}

int var_write_stdio(int stream , std::string text ) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL and vst->get_send_stdio() == true) {
        vst->write_stdio(stream, text) ;
    } else {
        if ( stream == 1 ) {
            fprintf( stdout , "%s" , text.c_str() ) ;
        } else {
            fprintf( stderr , "%s" , text.c_str() ) ;
        }
    }
    return(0) ;
}

int var_set_client_tag( std::string text ) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL) {
        // tag char declared length is 80
        if (text.length()>=80) {
            text.resize(79);
        }
        strcpy(vst->get_connection().client_tag, text.c_str());
#if __linux
#ifdef __GNUC__
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
        std::string short_str = std::string("VS_") + text.substr(0,12) ;
        pthread_setname_np(pthread_self(), short_str.c_str()) ;
#endif
#endif
#endif
    }
    return(0) ;
}

int var_send_list_size() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->send_list_size() ;
    }
    return(0) ;
}

int send_sie_resource() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->send_sie_resource() ;
    }
    return 0 ;
}

int send_sie_class() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->send_sie_class() ;
    }
    return 0 ;
}

int send_sie_enum() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        vst->send_sie_enum() ;
    }
    return 0 ;
}

int send_sie_top_level_objects() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        return vst->send_sie_top_level_objects() ;
    }
    return 0 ;
}

int send_file(std::string file_name) {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        return vst->send_file(file_name) ;
    }
    return 0 ;
}


// Command to turn on log to playback file
int var_server_log_on() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        return vst->set_log_on() ;
    }
    return 0 ;
}

// Command to turn off log to playback file
int var_server_log_off() {
    Trick::VariableServerThread * vst ;
    vst = get_vst() ;
    if (vst != NULL ) {
        return vst->set_log_off() ;
    }
    return 0 ;
}

Trick::VariableServer * var_server_get_var_server() {
    return the_vs ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::get_hostname
 * C wrapper Trick::VariableServer::get_hostname
 */
extern "C" const char * var_server_get_hostname() {
    return(the_vs->get_hostname()) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::get_port
 * C wrapper Trick::VariableServer::get_port
 */
extern "C" unsigned short var_server_get_port() {
    return(the_vs->get_listen_thread().get_port()) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::set_port
 * C wrapper Trick::VariableServer::set_port
 */
extern "C" void var_server_set_port(unsigned short port) {
    the_vs->get_listen_thread().set_port(port);
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::set_listen_socket
 * C wrapper Trick::VariableServer::set_listen_socket
 */
extern "C" void var_server_set_source_address(const char * source_address) {
    the_vs->get_listen_thread().set_source_address(source_address) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::get_user_tag
 * C wrapper Trick::VariableServer::get_user_tag
 */
extern "C" const char * var_server_get_user_tag() {
    return(the_vs->get_listen_thread().get_user_tag().c_str()) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::set_user_tag
 * C wrapper Trick::VariableServer::set_user_tag
 */
extern "C" void var_server_set_user_tag(const char * in_tag) {
    the_vs->get_listen_thread().set_user_tag(std::string(in_tag));
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::get_enabled
 * C wrapper Trick::VariableServer::get_enabled
 */
extern "C" int var_server_get_enabled() {
    return(the_vs->get_enabled()) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::set_enabled
 * C wrapper Trick::VariableServer::set_enabled
 */
extern "C" void var_server_set_enabled(int on_off) {
    the_vs->set_enabled((bool)on_off) ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::create_udp_socket
 * C wrapper Trick::VariableServer::create_udp_socket
 */
extern "C" int var_server_create_tcp_socket(const char * source_address, unsigned short port) {
    the_vs->create_tcp_socket(source_address, port) ;
    return 0 ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::create_udp_socket
 * C wrapper Trick::VariableServer::create_udp_socket
 */
extern "C" int var_server_create_udp_socket(const char * source_address, unsigned short port) {
    the_vs->create_udp_socket(source_address, port) ;
    return 0 ;
}

/**
 * @relates Trick::VariableServer
 * @copydoc Trick::VariableServer::create_multicast_socket
 * C wrapper Trick::VariableServer::create_multicast_socket
 */
extern "C" int var_server_create_multicast_socket(const char * mcast_address,
 const char * source_address, unsigned short port) {
    return the_vs->create_multicast_socket(mcast_address, source_address, port) ;
}

template<class T>
void var_set_value( V_DATA & v_data , T value ) ;

template<>
void var_set_value< double > ( V_DATA & v_data , double value ) {
    v_data.value.d = value ;
    v_data.type = TRICK_DOUBLE ;
}

template<>
void var_set_value< long long > ( V_DATA & v_data , long long value ) {
    v_data.value.ll = value ;
    v_data.type = TRICK_LONG_LONG ;
}

template<>
void var_set_value< const char * > ( V_DATA & v_data , const char * value ) {
    v_data.value.cp = (char *)value ;
    v_data.type = TRICK_STRING ;
}

template<>
void var_set_value< void * > ( V_DATA & v_data , void * value ) {
    v_data.value.vp = value ;
    v_data.type = TRICK_VOID_PTR ;
}

template<class T>
int var_set_base( const char  * var , T value , const char * units ) {
    REF2 *ref = ref_attributes((char *)var) ;
    if ( ref != NULL ) {
        if (ref->attr->io & TRICK_VAR_INPUT) {
            V_TREE v_tree ;
            V_DATA v_data ;
            v_tree.v_data = &v_data ;
            var_set_value( v_data , value) ;
            if ( units != NULL ) {
                ref->units = (char *)(map_trick_units_to_udunits(units).c_str()) ;
            } else {
                ref->units = (char *)units ;
            }
            ref_assignment(ref , &v_tree) ;
            free(ref) ;
        } else {
            message_publish(MSG_WARNING,"Cannot assign to %s because io_spec does not allow input\n", var) ;
        }
    }
    return 0 ;
}

int var_set( const char  * var , double value , const char * units ) {
    return var_set_base(var, value, units) ;
}

int var_set( const char  * var , long long value , const char * units ) {
    return var_set_base(var, value, units) ;
}

int var_set( const char  * var , const char * value , const char * units ) {
    return var_set_base(var, value, units) ;
}

int var_set( const char  * var , void * value , const char * units ) {
    return var_set_base(var, value, units) ;
}
