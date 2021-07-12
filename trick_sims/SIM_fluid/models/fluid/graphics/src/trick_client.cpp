#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"

// return socket file descriptor of Trick variable server
int setupComm() {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo("127.0.0.1", "42887", &hints, &servinfo)) != 0) {
		
	}
	//for(p = servinfo; p != NULL; p = p->ai_next) {
	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

		char pause_cmd[] = "trick.var_pause()\n";
	send(sockfd, pause_cmd, strlen(pause_cmd), 0);
	char ascii_cmd[] = "trick.var_ascii()\n";
	send(sockfd, ascii_cmd, strlen(ascii_cmd), 0);
	
	char var_cmd[] = "trick.var_add(\"dyn.fluid.NUM_PARTICLES\")\n";
	send(sockfd, var_cmd, strlen(var_cmd), 0);
	char unpause_cmd[] = "trick.var_unpause()\n";
	send(sockfd, unpause_cmd, strlen(unpause_cmd), 0);
	
	int BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	int numbytes = recv(sockfd, buf, BUF_SIZE, 0);
	buf[numbytes] = '\0';
	printf("Message: %s\n", buf);

	/* Parse number of particles */
	char* token = strtok(buf, "\t");
	int num_particles;

	while(token != NULL) {
		printf("%s\n", token);
		num_particles = atoi(token);
		token = strtok(NULL, "\t");
	}
	/* End parsing */ 
	send(sockfd, pause_cmd, strlen(pause_cmd), 0); // pause comm

	char clear_cmd[] = "trick.var_clear()\n";
	send(sockfd, clear_cmd, strlen(clear_cmd), 0);

	for(int i = 0; i < num_particles; i++) {
		char pos_x_cmd[BUF_SIZE];
		char pos_y_cmd[BUF_SIZE];
		char pos_z_cmd[BUF_SIZE];
		sprintf(pos_x_cmd, "trick.var_add(\"dyn.fluid.particlesArr[%d].pos[%d]\")\n", i, 0);
		sprintf(pos_y_cmd, "trick.var_add(\"dyn.fluid.particlesArr[%d].pos[%d]\")\n", i, 1);
		sprintf(pos_z_cmd, "trick.var_add(\"dyn.fluid.particlesArr[%d].pos[%d]\")\n", i, 2);

		send(sockfd, pos_x_cmd, strlen(pos_x_cmd), 0);
		send(sockfd, pos_y_cmd, strlen(pos_y_cmd), 0);
		send(sockfd, pos_z_cmd, strlen(pos_z_cmd), 0);
	}

	//sprintf(pos_y_cmd, "trick.var_add(\"dyn.fluid.particlesArr[%d].pos[%d]\")\n", i, 1);
		
	send(sockfd, unpause_cmd, strlen(unpause_cmd), 0); //unpause comm

	return sockfd;
}

std::vector<float> receiveParticlePositions(int sockfd) {
	std::vector<float> positions;
	
	int num_particles = 2048;

	int float_len = 14;
	int components = 3;
	int message_code = 1;
	int PARTICLE_BUF_SIZE = (num_particles * components + message_code) * float_len;
	char particle_pos_buf[PARTICLE_BUF_SIZE];
	

	// found experimentally
	int PACKET_SIZE = 8100;
	int offset = 0;
	int numbytes = recv(sockfd, particle_pos_buf + offset, PARTICLE_BUF_SIZE - offset, 0);
	printf("num bytes: %d | buf size: %d\n", numbytes, PARTICLE_BUF_SIZE);
	while (numbytes >= PACKET_SIZE) {
		
		offset += numbytes;
		numbytes = recv(sockfd, particle_pos_buf + offset, PARTICLE_BUF_SIZE - offset, 0);
		printf("num bytes: %d | buf size: %d\n", numbytes, PARTICLE_BUF_SIZE);
	}
	offset += numbytes;
	
	// 6786 full message
	particle_pos_buf[offset] = '\0';
	/* Parse particle positions */
	//printf("%s\n", particle_pos_buf);
	char* token = strtok(particle_pos_buf, "\t");
	token = strtok(NULL, "\t");
	
	while(token != NULL) {
		//printf("%s\n", token);
		float particle_pos = atof(token);
		positions.push_back(particle_pos);
		token = strtok(NULL, "\t");
	}
	/* End parsing positions*/ 
	for(int i = 0; i < 5; i++) {
		
		printf("Particle %d: (%f, %f, %f)\n", i, positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]);
	}


	printf("Size of positions: %d\n", positions.size());
	
	return positions;
}
/*
int main() {
	int sockfd = setupComm();
	
	receiveParticlePositions(sockfd);
	
	return 0;

	
}*/