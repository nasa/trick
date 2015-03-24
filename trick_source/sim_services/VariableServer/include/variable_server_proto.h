
#ifndef VARIABLE_SERVER_PROTO_H
#define VARIABLE_SERVER_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

const char * var_server_get_hostname() ;

unsigned short var_server_get_port() ;
void var_server_set_port(unsigned short port) ;
void var_server_set_source_address(const char * address) ;

const char * var_server_get_user_tag() ;
void var_server_set_user_tag(const char * tag) ;

int var_server_get_enabled() ;
void var_server_set_enabled(int on_off) ;

int var_server_create_tcp_socket(const char * address, unsigned short port) ;
int var_server_create_udp_socket(const char * address, unsigned short port) ;
int var_server_create_multicast_socket(const char * mcast_address, const char * address, unsigned short port) ;

#ifdef __cplusplus
}
#endif

#endif
