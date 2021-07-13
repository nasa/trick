#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

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

    if((status = getaddrinfo("127.0.0.1", "12345", &hints, &servinfo)) != 0) {

    }
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    
    int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    printf("before recv\n");
    int numbytes = recv(sockfd, buf, BUF_SIZE, 0);
    printf("after recv\n");
    buf[numbytes] = '\0';
    printf("%s\n", buf);
    while (numbytes > 0) {
        memset(buf, 0, BUF_SIZE);
        numbytes = recv(sockfd, buf, BUF_SIZE, 0);
        buf[numbytes] = '\0';
        printf("%s\n", buf);
    }
    

    return 0;
}