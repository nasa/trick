/********************************* TRICK HEADER *******************************
PURPOSE: (Create a TCP server to transmit particle data to client)
LIBRARY DEPENDENCY:
	((FluidServer.o))
*******************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include "../include/FluidServer.hh"
#define BACKLOG 10

void FluidServer::initConnection() {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int sockfd, newfd;
	
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "12345", &hints,  &servinfo)) != 0) {
		
	}
	//for(p = servinfo; p != NULL; p = p->ai_next) {
	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(sockfd, BACKLOG);
	//}
	struct sockaddr their_addr;
	socklen_t addr_len = sizeof(their_addr);
	newfd = accept(sockfd, &their_addr, &addr_len);
	freeaddrinfo(servinfo);
	clientfd = newfd;
}

void FluidServer::sendParticleData(Fluid* fluid) {


	// one packet sent containing all particle position data
	int PACKET_SIZE = fluid->NUM_PARTICLES * 3 * sizeof(float);
	char packet[PACKET_SIZE];
	int packet_offset = 0;
	/*
	for (int i = 0; i < fluid->NUM_PARTICLES; i++) {
		memcpy(packet + packet_offset, &fluid->particles[i].pos[0], sizeof(float));
		packet_offset += sizeof(float);
		memcpy(packet + packet_offset, &fluid->particles[i].pos[1], sizeof(float));
		packet_offset += sizeof(float);
		memcpy(packet + packet_offset, &fluid->particles[i].pos[2], sizeof(float));
		packet_offset += sizeof(float);
	}*/

	int bytes_sent = send(clientfd, packet, PACKET_SIZE, 0); 
	send(clientfd, packet + bytes_sent, PACKET_SIZE - bytes_sent, 0);
	
	packet_offset = 0;
	
	
	
	
}
