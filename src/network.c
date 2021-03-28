#include "../include/protocol.h"   

// ##### NETWORK ############################

void *ThreadMainTCP(void *threadArgs) {
    int clntSock, recvMsgSize, client_state = 0;

    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              

    if ((recvMsgSize = recv(clntSock, &client_state, sizeof(client_state), 0)) < 0)
        DieWithError("recv() failed");

		pthread_mutex_lock(&mutex);
        if(client_state == 0 || client_state == 3) {
            get_hard_info();
            get_sys_info();
            if(send(clntSock, &hard_info, sizeof(hard_info), 0) != sizeof(hard_info))	
			    DieWithError("send() sent a different number of bytes than expected");

            if(send(clntSock, &mem, sizeof(mem), 0) != sizeof(mem))	
			    DieWithError("send() sent a different number of bytes than expected");

            if(send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info))
                DieWithError("send() sent a different number of bytes than expected");
        }
		if(client_state == 1) {
            get_hard_info();
            if(send(clntSock, &hard_info, sizeof(hard_info), 0) != sizeof(hard_info))	
			    DieWithError("send() sent a different number of bytes than expected");

            if(send(clntSock, &mem, sizeof(mem), 0) != sizeof(mem))
                DieWithError("send() sent a different number of bytes than expected");
        }
		if(client_state == 2) {
            get_sys_info();
            if(send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info))	
			    DieWithError("send() sent a different number of bytes than expected");

            if(send(clntSock, &mem, sizeof(mem), 0) != sizeof(mem))
                DieWithError("send() sent a different number of bytes than expected");
        }
		pthread_mutex_unlock(&mutex);

    return (NULL);
}

void *ThreadMainUDP(void *threadArgs) {
    unsigned int cliAddrLen;
    int clntSock, recvMsgSize, sendTmp, client_state = 0;
    struct sockaddr_in echoClntAddr;        
    
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              

    cliAddrLen = sizeof(echoClntAddr);
    if ((recvMsgSize = recvfrom(clntSock, &client_state, sizeof(client_state), 0,
        (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        DieWithError("recvfrom() failed");

        pthread_mutex_lock(&mutex);
		if(client_state == 0 || client_state == 3) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		if(client_state == 1) sendTmp = send(clntSock, &hard_info, sizeof(hard_info), 0) != sizeof(hard_info);
		if(client_state == 2) sendTmp = send(clntSock, &sys_info, sizeof(sys_info), 0) != sizeof(sys_info);
		pthread_mutex_unlock(&mutex);
    
        if(sendTmp)	
			DieWithError("send() sent a different number of bytes than expected");

    return (NULL);
}

void TCPWay(int port, int client_count, pthread_t* threadID) {
    int sock, clntSock;                    
    unsigned int clntLen;           
    struct sockaddr_in echoServAddr;        
    struct sockaddr_in echoClntAddr;        

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    memset(&echoServAddr, 0, sizeof(echoServAddr));  
    echoServAddr.sin_family = PF_INET;               
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(port);             

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    if (listen(sock, client_count) < 0)
        DieWithError("listen() failed");

    for(;;){
        for (int i = 0; i < client_count; i++){
            clntLen = sizeof(echoClntAddr);
            if ((clntSock = accept(sock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
                DieWithError("accept() failed");

            if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) 
                   == NULL)
                DieWithError("malloc() failed");
            threadArgs -> clntSock = clntSock;

            if (pthread_create(&threadID[i], NULL, ThreadMainTCP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            
            if(pthread_join(threadID[i], NULL) != 0) 
                DieWithError("Joining the second thread");
        }
    }
}

void UDPWay(int port, int client_count, pthread_t* threadID) {
    int sock;
    struct sockaddr_in echoServAddr;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));   
    echoServAddr.sin_family = PF_INET;
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

            if (pthread_create(&threadID[i], NULL, ThreadMainUDP, (void *) threadArgs) != 0)
                DieWithError("pthread_create() failed");
            
            if(pthread_join(threadID[i], NULL) != 0) {
                perror("Joining the second thread");
                exit(-1);
            }
        }
    }
}