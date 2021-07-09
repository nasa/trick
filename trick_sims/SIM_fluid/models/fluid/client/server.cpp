#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>

#define BACKLOG 10

int main() {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int sockfd, newfd;
	
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "3490", &hints,  &servinfo)) != 0) {
		
	}
	//for(p = servinfo; p != NULL; p = p->ai_next) {
		sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
		bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
		listen(sockfd, BACKLOG);
	//}
	struct sockaddr their_addr;
	socklen_t addr_len = sizeof(their_addr);
	newfd = accept(sockfd, &their_addr, &addr_len);
	
	int bytes_sent = send(newfd, "Hello from server!", 19, 0); 
	freeaddrinfo(servinfo);
	return 0;
}