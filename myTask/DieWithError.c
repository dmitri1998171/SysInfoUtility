#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#include <string.h>
#include "protocol.h"

struct info info;

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void out(){
    printf("Net Int.:        ");
	for(int i=0; i < info.count; i++){	
		printf("%s ", info.net_int[i]);
	}

	printf("\n%s", info.mem.memTotal);
	printf("%s", info.mem.memFree);
	printf("%s", info.mem.memAvail);

	printf("CPU:\t%s \n", info.cpuavg);
}
