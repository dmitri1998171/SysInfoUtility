#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for close() */
#include <string.h>
#include "protocol.h"

FILE *fp;
struct info info;

void openFunc(char name[]){
	if ((fp = fopen(name, "r")) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

void get_info() {
/////////////////INTERFACES/////////////////////
	system("rm ./tmp.txt && ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFunc(name0);

        // очистка массива для перезаписи
    memset(info.net_int[N], 0, sizeof(info.net_int[N]));    
    
	while (!feof(fp)){
		fscanf(fp, "%s", info.net_int[info.count]);
		info.count++;
	}
    info.count -= 1;    // исключает ошибку feof - дублирует последнюю строку
	fclose(fp);
//////////////MEMORY////////////////////////////
	char name1[] = "/proc/meminfo";
	openFunc(name1);
	
	fgets(info.mem.memTotal, N, fp);
	fgets(info.mem.memFree, N, fp);
	fgets(info.mem.memAvail, N, fp);
	fclose(fp);
/////////////////CPU////////////////////////////
	char name2[] = "/proc/loadavg";
	openFunc(name2);

	fgets(info.cpuavg, N, fp);
	fclose(fp);
}
