// #include <sys/cdefs.h>
// #include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>
#define BACKLOG 32

void error(const char *msg);
int main(int argc, char *argv[]){
    int sockfd, portno;
    struct addrinfo hints, *servinfo; 
    int status, ch, bytes_sent;
    int npackets=0, alarmtimeout=1000;
    char host[BACKLOG], *port;
    char msg[BACKLOG]="";
    bool keepsending = true;
    struct timeval ts1, ts2;

    if (argc < 2)
        error("ERROR, no port provided");
    while ((ch = getopt(argc, argv, "n:t:")) != EOF){
        switch(ch){
        case 'n':
            npackets = atoi(optarg);
            if (npackets < 0)
                error("ERROR, bad number of packets to transmit");
            if (npackets!=0)
                keepsending = false;
        break;

        case 't':
            alarmtimeout = atoi(optarg);
            if ((alarmtimeout < 1) || (alarmtimeout == INT_MAX))
                error("ERROR invalid timeout");
            break;
        break;
        }
    }
    argc -= optind;
    argv += optind;
    if(argc<1)
        error("ERROR, no server input");

    for(int i=0; i<argc; i++){
        port = strchr(argv[i], ':')+1;
        portno = atoi(port);
        strncpy(host, argv[i], strlen(argv[i]) - strlen(port)-1);
        memset(&hints, 0, sizeof hints); 
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        status = getaddrinfo(host, port, &hints, &servinfo);
        if (status != 0)
            error("ERROR, no such host");
        // sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);  
        if (sockfd < 0)
            error("ERROR opening socket");
        if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) 
            error("ERROR connecting");

        double timeout = 0.;
        double rtt = 0.;
        int ct = 0;
        struct sockaddr_in *addr;
		addr = (struct sockaddr_in *)servinfo->ai_addr; 
        if(keepsending){
            while(timeout <= alarmtimeout){
                // printf("ct=%d\n", ct);
                gettimeofday(&ts1, NULL);
                send(sockfd, msg, sizeof(msg), 0);
                recv(sockfd, msg, sizeof(msg), 0);
                gettimeofday(&ts2, NULL);
                rtt = 1e3*(ts2.tv_sec - ts1.tv_sec) + 1e-3 * (ts2.tv_usec - ts1.tv_usec);
                timeout += rtt;
                printf("recvfrom %s, RTT=%.3fmsec\n", inet_ntoa((struct in_addr)addr->sin_addr), rtt);
                // printf("timeout=%.3f\n", timeout);
                ct++;
            }
        }
        else{
        	while(ct<npackets && timeout <= alarmtimeout){
                // printf("ct=%d\n", ct);
                gettimeofday(&ts1, NULL);
                send(sockfd, msg, sizeof(msg), 0);
                recv(sockfd, msg, sizeof(msg), 0);
                gettimeofday(&ts2, NULL);
                rtt = 1e3*(ts2.tv_sec - ts1.tv_sec) + 1e-3 * (ts2.tv_usec - ts1.tv_usec);
                timeout += rtt;
                printf("recvfrom %s, RTT=%.3fmsec\n", inet_ntoa((struct in_addr)addr->sin_addr), rtt);
                // printf("timeout=%.3f\n", timeout);
                ct++;
            }
        }
        freeaddrinfo(servinfo);
        close(sockfd);

    }
    return 0; 
}

void error(const char *msg){
    perror(msg);
    exit(1);
}