#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     
#include "protocol.h"   

void ClientTCPWay(char *servIP){
    char getInfo[20] = "GET_SYSTEM_INFO";
    int sock, state;
    struct sockaddr_in echoServAddr;        

    if (( sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));     
    echoServAddr.sin_family      = AF_INET;             
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   
    echoServAddr.sin_port        = htons(port);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
    if (send(sock, &getInfo, sizeof(getInfo), 0) != sizeof(getInfo))
        DieWithError("send() sent a different number of bytes than expected");

    if ((recv(sock, &state, sizeof(state), 0)) < 0)
        DieWithError("recv() failed");

    if(state == 0 || state == 3) {
        if ((recv(sock, &hard_info, sizeof(hard_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        out();
        if ((recv(sock, &sys_info, sizeof(sys_info), 0)) < 0)
            DieWithError("recv() failed.");
        current_values_output();
    }
    if(state == 1) {
        if ((recv(sock, &hard_info, sizeof(hard_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        out();
    }
    if(state == 2) {
        if ((recv(sock, &sys_info, sizeof(sys_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        current_values_output();
    }

    close(sock);
}

void ClientUDPWay(char *servIP){
    char getInfo[20] = "GET_SYSTEM_INFO";
    int echoStringLen, sock;               /* Length of string to echo */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    struct sockaddr_in echoServAddr;        /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */

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
    if (recvfrom(sock, &sys_info, sizeof(sys_info), 0, (struct sockaddr *) &fromAddr, &fromSize) < 0)
        DieWithError("recvfrom() failed");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }

    current_values_output();
	// out();      // вывод на экран

    close(sock);
    exit(0);
}

int checkIP(char buffer[]){
    int CountIp = 0;
    char* NotInt;
    int current; 
    int isIp = 1;
    int cnt =0; 
    char b[20];
    int IP[4]; 
    
    memset(b,0,20); 

    for (int i=0; i <= strlen(buffer); i++){
        if(buffer[i]=='.' || buffer[i] == 0){
            current =strtol(b,&NotInt,10); 
            if ((!*NotInt && CountIp < 4) && (current >=0 && current <256)){    
                IP[CountIp] = current; 
            } 
            else{ isIp = 0; break; }
            
            memset(b,0,20);
            cnt=0; 
            CountIp++;
        }
        else{ b[cnt++] = buffer[i]; }
    }

    if (isIp)return 1;
    else return 0;
}

int main(int argc, char *argv[]){
    if (argc != 4){    
       fprintf(stderr, "Usage: %s <Server IP> <Echo Port> <TCP/UDP>\n", argv[0]);
       exit(1);
    }

    // Парсинг и валидация аргументов 

    char *servIP = argv[1];       
    port = atoi(argv[2]);         
    strcpy(type_proto, argv[3]);

    // --------------------------

    if (!checkIP(servIP)){
        printf("Invalid IP\n");
        exit(1);
    }

    if(port < 1024 || port > 65535){
        printf("Invalid port\n");
        exit(1);
    }
    // ==========================

    if(strcmp(type_proto, "TCP")==0 || strcmp(type_proto, "tcp")==0)  
        ClientTCPWay(servIP);
    else if(strcmp(type_proto, "UDP")==0 || strcmp(type_proto, "udp")==0)  
        ClientUDPWay(servIP);
    else { printf("Invalid protocol type\n"); exit(1); }

    exit(0);
}
