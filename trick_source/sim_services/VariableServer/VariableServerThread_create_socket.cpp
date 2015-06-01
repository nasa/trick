
#include <stdlib.h>
#include "trick/VariableServerThread.hh"
#include "trick/tc_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

int Trick::VariableServerThread::create_udp_socket(const char * address, unsigned short in_port) {
    listen_dev = &connection ;
    tc_init_with_connection_info(&connection, AF_INET, SOCK_DGRAM, address, in_port) ;
    message_publish(MSG_INFO, "Created udp variable server %s:%d\n", listen_dev->hostname, listen_dev->port) ;
    tc_blockio( &connection , TC_COMM_NOBLOCKIO ) ;
    conn_type = UDP ;
    return 0 ;
}

int Trick::VariableServerThread::create_mcast_socket(const char * mcast_address, const char * address, unsigned short in_port) {
    int ret ;
    if ( mcast_address == NULL || mcast_address[0] == '\0' ) {
        message_publish(MSG_ERROR, "Multicast address must be defined.\n") ;
        return -1 ;
    }
    create_udp_socket(address, in_port) ;

    conn_type = MCAST ;

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(mcast_address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(connection.socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) ;
    if ( ret < 0 ) {
        perror("ip_add_membership") ;
        return ret ;
    }

    memset(&connection.remoteServAddr, 0, sizeof(struct sockaddr_in));
    connection.remoteServAddr.sin_family = AF_INET;
    connection.remoteServAddr.sin_addr.s_addr = inet_addr(mcast_address);
    connection.remoteServAddr.sin_port = htons(in_port);

    message_publish(MSG_INFO, "Multicast variable server output %s:%d\n", mcast_address, listen_dev->port) ;
    return 0 ;

}
