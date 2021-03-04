#include "protocol.h"   

void ClientTCPWay(char *serverIP, int port, int client_state) {
    char getInfo[20] = "GET_SYSTEM_INFO";
    int sock;
    struct sockaddr_in echoServAddr;        

    if (( sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));     
    echoServAddr.sin_family      = PF_INET;             
    echoServAddr.sin_addr.s_addr = inet_addr(serverIP);   
    echoServAddr.sin_port        = htons(port);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
    if (send(sock, &client_state, sizeof(client_state), 0) != sizeof(client_state))
        DieWithError("send() sent a different number of bytes than expected");

    if(client_state == 0 || client_state == 3) {
        if ((recv(sock, &hard_info, sizeof(hard_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        out();
        if ((recv(sock, &sys_info, sizeof(sys_info), 0)) < 0)
            DieWithError("recv() failed.");
        current_values_output();
    }
    if(client_state == 1) {
        if ((recv(sock, &hard_info, sizeof(hard_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        out();
    }
    if(client_state == 2) {
        if ((recv(sock, &sys_info, sizeof(sys_info), 0)) < 0)
            DieWithError("recv() failed.");
        if ((recv(sock, &mem, sizeof(mem), 0)) < 0)
            DieWithError("recv() failed.");
        current_values_output();
    }

    close(sock);
}

void ClientUDPWay(char *serverIP, int port) {
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
    echoServAddr.sin_addr.s_addr = inet_addr(serverIP);  /* Server IP address */
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

int checkIP(char buffer[]) {
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

void getInfo(char *type_proto, char *serverIP, int port, int state) {
    if(strcmp(type_proto, "TCP") == 0 || strcmp(type_proto, "tcp") == 0)  
        ClientTCPWay(serverIP, port, state);
    else if(strcmp(type_proto, "UDP") == 0 || strcmp(type_proto, "udp") == 0)  
        ClientUDPWay(serverIP, port);
    else DieWithError("Invalid protocol type\n");
}

int main(int argc, char *argv[]) {
    // Парсинг и валидация аргументов 
    if (argc < 4 || argc > 8){    
       fprintf(stderr, "Usage: %s <Server IP> <Echo Port> <TCP/UDP> [OPTION]\n", argv[0]);
       exit(1);
    }

    int res, optIdx, state = 0;
    static struct option long_opt[] = {
        {"hard_info", 0, 0, 'w'},
        {"sys_info", 0, 0, 's'},
        {"log", 0, 0, 'l'},
        {"html", 0, 0, 'h'},
        {"network", 1, 0, 'n'},
        {"help", 0, 0, '?'},
        {0,0,0,0}
    };

    char *serverIP = argv[1];       
    if (!checkIP(serverIP)) {
        DieWithError("Invalid IP\n"); }

    int port = atoi(argv[2]);         
    if(port < 1024 || port > 65535) {
        DieWithError("Invalid port\n"); }
    
    char type_proto[4];
    strcpy(type_proto, argv[3]);

    while ((res = getopt_long(argc,argv,"wslh?", long_opt, &optIdx)) != -1) {
        switch(res) {
            case 'w': { state += 1; getInfo(type_proto, serverIP, port, state); break; }	// Вывод хар-к ПК
            case 's': { state += 2; getInfo(type_proto, serverIP, port, state); break; }	// Вывод текущ. знач.
            case 'l': { getInfo(type_proto, serverIP, port, state); write_to_log(); break; }	// Запись в лог
            case 'h': { getInfo(type_proto, serverIP, port, state); write_to_log(); generate_html(); break; }	// Запись в html
            case '?': { printf("\n Usage: %s <Server IP> <Echo Port> <TCP/UDP> [OPTION]\n\n -w, --hard_info print hardware information\n -s, --sys_info  print system information\n -l, --log       output to \'sysInfo.log\' file\n -h, --html\t output to html\n -?, --help\t print this help\n\n By default, without options, the utility displays hardware and system information together\n\n", argv[0]); break; }
        }
    }

    exit(0);
}
