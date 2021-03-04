#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main (int argc, char *argv[])
{
    char type_proto[3];

    printf("protocol type(TCP/UDP): ");
    scanf("%s", type_proto);
    
    if(strcmp(type_proto, "TCP") == 0 || strcmp(type_proto, "tcp") == 0) { 
        // TCPWay(port, client_count, state); 
        printf("TCP!!!\n");
    }
    else if(strcmp(type_proto, "UDP") == 0 || strcmp(type_proto, "udp") == 0) { 
        // UDPWay(port, client_count, state); 
        printf("UDP!!!\n");
    }
    else { printf("Invalid protocol type\n"); exit(1); }
 
return(0);
}