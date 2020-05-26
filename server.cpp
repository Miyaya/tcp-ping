// #include <sys/cdefs.h>
// #include <netinet/ip.h>
// #include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#define BACKLOG 32

void error(const char *msg);
in_port_t get_in_port(struct sockaddr *sa);
int main(int argc, char *argv[]){
    int sockfd, clisockfd;
    struct addrinfo hints, *clinfo, *c; 
    socklen_t clilen;
    int status, bytes_sent;
    char *host, *port, msg[BACKLOG];
    void *buf;

    if(argc < 2)
        error("ERROR, no port provided\n");
    memset(&hints, 0, sizeof hints); 
    port = argv[1];
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &clinfo);
    if(status!=0)
        error("ERROR, no such host\n");
    for(c=clinfo; c!=NULL; c=c->ai_next){
	    sockfd = socket(c->ai_family, c->ai_socktype, c->ai_protocol);  
	    if(sockfd < 0){
	        perror("socket");
	        continue;
	    }
	    if(bind(sockfd, c->ai_addr, c->ai_addrlen)<0){
	        perror("binding");
	        continue;
	    }
	    if(listen(sockfd, BACKLOG) < 0){
	        perror("listen");
	        continue;
	    }
	    clisockfd = accept(sockfd, c->ai_addr, &c->ai_addrlen);

	    struct sockaddr_in *addr;
	    addr = (struct sockaddr_in *)c->ai_addr; 

	    while(recv(clisockfd, buf, sizeof buf, 0)){
	        printf("recv from %s:%d\n", inet_ntoa((struct in_addr)addr->sin_addr), get_in_port(c->ai_addr));
	        if(send(clisockfd, msg, sizeof(msg), 0)==-1)
	            break;
	    }
    }
    freeaddrinfo(clinfo);
    close(clisockfd);
    close(sockfd);
    
    return 0; 
}

void error(const char *msg){
    perror(msg);
    exit(1);
}

in_port_t get_in_port(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}
