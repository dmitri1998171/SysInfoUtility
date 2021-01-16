#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "protocol.h"   

int sock;                               /* Socket descriptor */
struct sockaddr_in echoServAddr;        /* Echo server address */
struct info info;
char *servIP;
char getInfo[20] = "Get info!";

void DieWithError(char *errorMessage);  /* Error handling function */
void out(void);

void TCPWay(){
    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(port);         /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
    if (send(sock, &getInfo, sizeof(getInfo), 0) != sizeof(getInfo))
        DieWithError("send() sent a different number of bytes than expected");

    if ((recv(sock, &info, sizeof(info), 0)) < 0)
        DieWithError("recv() failed");
    
	out();  // вывод на экран

    close(sock);
}

void UDPWay(){
    int echoStringLen;               /* Length of string to echo */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(port);     /* Server port */

    /* Send the string to the server */
    echoStringLen = strlen(getInfo);
    if (sendto(sock, getInfo, echoStringLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected");
  
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if (recvfrom(sock, &info, sizeof(info), 0, (struct sockaddr *) &fromAddr, &fromSize) < 0)
        DieWithError("recvfrom() failed");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }

	out();      // вывод на экран

    close(sock);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 4)    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Echo Port> <TCP/UDP>\n", argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    port = atoi(argv[2]);         /* Use given port, if any */
    type_proto = argv[3];

    if(strcmp(type_proto, "TCP")==0 || strcmp(type_proto, "tcp")==0) { 
        printf("TCPWay\n"); 
        TCPWay();
        }
    else if(strcmp(type_proto, "UDP")==0 || strcmp(type_proto, "udp")==0) { 
        printf("UDPWay\n"); 
        UDPWay();
        }
    else { printf("Invalid protocol type!\n"); exit(1); }

    exit(0);
}
