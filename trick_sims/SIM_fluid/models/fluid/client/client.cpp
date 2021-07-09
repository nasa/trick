#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include <vector>

std::vector<float> receiveParticlePositions(int sockfd) {
	std::vector<float> positions;
	
	char* pause_cmd = "trick.var_pause()\n";
	send(sockfd, pause_cmd, strlen(pause_cmd), 0);
	char* ascii_cmd = "trick.var_ascii()\n";
	send(sockfd, ascii_cmd, strlen(ascii_cmd), 0);
	char* var_cmd = "trick.var_add(\"dyn.fluid.NUM_PARTICLES\")\n";
	send(sockfd, var_cmd, strlen(var_cmd), 0);
	char* unpause_cmd = "trick.var_unpause()\n";
	send(sockfd, unpause_cmd, strlen(unpause_cmd), 0);
	
	int BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	int numbytes = recv(sockfd, buf, BUF_SIZE, 0);
	buf[numbytes] = '\0';
	printf("Message: %s\n", buf);
	
	return positions;
}

int main() {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int sockfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo("127.0.0.1", "36775", &hints, &servinfo)) != 0) {
		
	}
	//for(p = servinfo; p != NULL; p = p->ai_next) {
	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	
	receiveParticlePositions(sockfd);
	
	return 0;

	
}