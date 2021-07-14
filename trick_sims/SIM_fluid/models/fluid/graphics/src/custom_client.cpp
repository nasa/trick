#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include "custom_client.h"

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

    if((status = getaddrinfo("127.0.0.1", "12345", &hints, &servinfo)) != 0) {

    }
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);


    return sockfd;

}
std::vector<float> receiveParticlePositions(int sockfd) {
    std::vector<float> positions;

    int NUM_PARTICLES = 2048;
    int DIMENSIONS = 3;
    int BUF_SIZE = NUM_PARTICLES * DIMENSIONS * sizeof(float);

    char buf[BUF_SIZE];

    printf("before recv\n");
    int numbytes = recv(sockfd, buf, BUF_SIZE, 0);
    printf("after recv: %d bytes\n", numbytes);

    /* */
    
    for (int i = 0; i < numbytes; i += sizeof(float)) {
        float position;
        memcpy(&position, buf + i, sizeof(float));
        positions.push_back(position);
        //printf("%f\n", position);
    }
    printf("num bytes: %d \n", numbytes);

    if (numbytes != BUF_SIZE) {
        int numbytes2 = recv(sockfd, buf + numbytes, BUF_SIZE - numbytes, 0);
        printf("after recv2: %d bytes\n", numbytes2);

        for (int i = 0; i < numbytes2; i += sizeof(float)) {
            float position;
            memcpy(&position, buf + numbytes + i, sizeof(float));
            positions.push_back(position);
            //printf("%f\n", position);
        }
    }

    return positions;

}