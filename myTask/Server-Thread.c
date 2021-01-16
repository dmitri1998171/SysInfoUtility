#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>
#include "protocol.h"   

pthread_t* threadID;
struct ThreadArgs *threadArgs;          /* Pointer to argument structure for thread */

struct sockaddr_in echoServAddr;        /* Local address */
struct sockaddr_in echoClntAddr;        /* Client address */

struct info info;

unsigned int cliAddrLen;                /* Length of incoming message */

int recvMsgSize;                        /* Size of received message */
char echoBuffer[ECHOMAX];               /* Buffer for echo string */

void DieWithError(char *errorMessage);  /* Error handling function */
void get_info(void);
void out(void);
void *ThreadMainTCP(void *arg);         /* Main program of a thread */
void *ThreadMainUDP(void *arg);         /* Main program of a thread */

/* Structure of arguments to pass to client thread */
struct ThreadArgs
{
    int clntSock;                       /* Socket descriptor for client */
};

void TCPWay(){
    int sock;                        /* Socket */
    int clntSock;                    /* Socket descriptor for client */
    unsigned int clntLen;            /* Length of client address data structure */
    
    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(sock, max_clnt) < 0)
        DieWithError("listen() failed");

    for(;;){
        for (int i = 0; i < max_clnt; i++){
            clntLen = sizeof(echoClntAddr);
            /* Wait for a client to connect */
            if ((clntSock = accept(sock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
                DieWithError("accept() failed");

            if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) 
                   == NULL)
                DieWithError("malloc() failed");
            threadArgs -> clntSock = clntSock;

            /* Create client thread */
            if (pthread_create(&threadID[i], NULL, ThreadMainTCP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            printf("with thread %ld\n", (long int) threadID);
        }
    }
}

void UDPWay(){
    int sock;                        /* Socket */
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    for(;;){
        for (int i = 0; i < max_clnt; i++){
            if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) 
                   == NULL)
                DieWithError("malloc() failed");
            threadArgs -> clntSock = sock;

            if (pthread_create(&threadID[i], NULL, ThreadMainUDP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            printf("with thread %ld\n", (long int) threadID);

            if(pthread_join(threadID[i], NULL) != 0) {
                perror("Joining the second thread");
                exit(-1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <client count> <Server Port> <TCP/UDP>\n", argv[0]);
        exit(1);
    }

    max_clnt = atoi(argv[1]);
    port = atoi(argv[2]);  
    type_proto = argv[3];

    threadID = (pthread_t*) malloc(max_clnt * sizeof(pthread_t));

    if(strcmp(type_proto, "TCP")==0 || strcmp(type_proto, "tcp")==0) { 
        printf("TCPWay\n"); 
        TCPWay();
        }
    else if(strcmp(type_proto, "UDP")==0 || strcmp(type_proto, "udp")==0) { 
        printf("UDPWay\n"); 
        UDPWay();
        }
    else { printf("Invalid protocol type!\n"); exit(1); }
}

void *ThreadMainTCP(void *threadArgs)
{
    int clntSock;        /* Socket descriptor for client connection */

    /* Guarantees that thread resources are deallocated upon return */
    pthread_detach(pthread_self()); 

    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              /* Deallocate memory for argument */

    /* clntSock is connected to a client! */
    if ((recvMsgSize = recv(clntSock, echoBuffer, 32, 0)) < 0)
        DieWithError("recv() failed");

    echoBuffer[recvMsgSize]='\0';
    printf("recv: %s\n\n", echoBuffer);

    get_info();         // сбор информации
    out();              // вывод на экран

    if (send(clntSock, &info, sizeof(info), 0) != sizeof(info))
        DieWithError("send() sent a different number of bytes than expected");
    printf("\n\n");
    
    return (NULL);
}

void *ThreadMainUDP(void *threadArgs)
{
    int clntSock;        /* Socket descriptor for client connection */

    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              /* Deallocate memory for argument */

    /* Block until receive message from a client */
    cliAddrLen = sizeof(echoClntAddr);
    if ((recvMsgSize = recvfrom(clntSock, echoBuffer, ECHOMAX, 0,
        (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        DieWithError("recvfrom() failed");

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    echoBuffer[recvMsgSize]='\0';
    printf("Recv: %s\n\n", echoBuffer);

    get_info();         // сбор информации
    out();              // вывод на экран

    /* Send received datagram back to the client */
    if (sendto(clntSock, &info, sizeof(info), 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(info))
        DieWithError("sendto() sent a different number of bytes than expected");
    printf("\n\n");

    return (NULL);
}