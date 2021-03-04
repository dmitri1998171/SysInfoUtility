#include "protocol.h"   

// ##### NETWORK ############################

void *ThreadMainTCP(void *threadArgs) {
    char echoBuffer[ECHOMAX];
    unsigned int cliAddrLen;
    int clntSock, recvMsgSize, state, sendTmp;                       

    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    state = ((struct ThreadArgs *) threadArgs) -> state;
    free(threadArgs);              /* Deallocate memory for argument */

    /* clntSock is connected to a client! */
    if ((recvMsgSize = recv(clntSock, echoBuffer, ECHOMAX, 0)) < 0)
        DieWithError("recv() failed");

    // Проверка на получение правильной команды от клиента
    if(strcmp(echoBuffer, "GET_SYSTEM_INFO") == 0){
        echoBuffer[recvMsgSize]='\0';
        printf("Recv: %s\n\n", echoBuffer);

		get_sys_info();

		pthread_mutex_lock(&mutex);
		if(state == 0 || state == 3) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		if(state == 1) sendTmp = send(clntSock, &hard_info, sizeof(hard_info), 0) != sizeof(hard_info);
		if(state == 2) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		pthread_mutex_unlock(&mutex);

		if(sendTmp)	
			DieWithError("send() sent a different number of bytes than expected");
    }
    else
        printf("ERROR! Recieved bad client command!\n");
    
    return (NULL);
}

void *ThreadMainUDP(void *threadArgs) {
    char echoBuffer[ECHOMAX];
    int clntSock, recvMsgSize, state, sendTmp;
    unsigned int cliAddrLen;
    struct sockaddr_in echoClntAddr;        
    
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    state = ((struct ThreadArgs *) threadArgs) -> state;
    free(threadArgs);              

    cliAddrLen = sizeof(echoClntAddr);
    if ((recvMsgSize = recvfrom(clntSock, echoBuffer, ECHOMAX, 0,
        (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        DieWithError("recvfrom() failed");

    if(strcmp(echoBuffer, "GET_SYSTEM_INFO") == 0){
        echoBuffer[recvMsgSize]='\0';
        printf("Recv: %s\n\n", echoBuffer);

        pthread_mutex_lock(&mutex);
		if(state == 0 || state == 3) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		if(state == 1) sendTmp = send(clntSock, &hard_info, sizeof(hard_info), 0) != sizeof(hard_info);
		if(state == 2) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		pthread_mutex_unlock(&mutex);
    
        if(sendTmp)	
			DieWithError("send() sent a different number of bytes than expected");

        printf("\n\n");
    }
    else
        printf("ERROR! Recieved bad client command!\n");
    
    return (NULL);
}

void TCPWay(int port, int max_clnt, int state) {
    int sock, clntSock;                    
    unsigned int clntLen;           
    struct sockaddr_in echoServAddr;        
    struct sockaddr_in echoClntAddr;        

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    memset(&echoServAddr, 0, sizeof(echoServAddr));  
    echoServAddr.sin_family = AF_INET;               
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(port);             

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    if (listen(sock, max_clnt) < 0)
        DieWithError("listen() failed");

    for(;;){
        for (int i = 0; i < max_clnt; i++){
            clntLen = sizeof(echoClntAddr);
            if ((clntSock = accept(sock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
                DieWithError("accept() failed");

            if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) 
                   == NULL)
                DieWithError("malloc() failed");
            threadArgs -> clntSock = clntSock;
			threadArgs -> state = state;

            if (pthread_create(&threadID[i], NULL, ThreadMainTCP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            
            if(pthread_join(threadID[i], NULL) != 0) {
                perror("Joining the second thread");
                exit(-1);
            }
        }
    }
}

void UDPWay(int port, int max_clnt, int state) {
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));   
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    for(;;){
        for (int i = 0; i < client_count; i++){
            if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) 
                   == NULL)
                DieWithError("malloc() failed");
            threadArgs -> clntSock = sock;
			threadArgs -> state = state;

            if (pthread_create(&threadID[i], NULL, ThreadMainUDP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            
            if(pthread_join(threadID[i], NULL) != 0) {
                perror("Joining the second thread");
                exit(-1);
            }
        }
    }
}