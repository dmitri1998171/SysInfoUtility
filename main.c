#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define N 50
#define ECHOMAX 20

int max_clnt, port;
char *type_proto;

struct mem {
	char memTotal[N];	//
	char memFree[N];	// ОЗУ
	char memAvail[N];	//
};

struct info {
	char cpuavg[N];		  // Нагрузка процессора
	struct mem mem;		  // ОЗУ
};

struct sys_info {
    char version[N];      // Версия Линукс
    char kernel[N];       // Ядро
	char net_int[N][N];	  // Сетевые интерфейсы
	int count;			  // счетчик кол-ва сетев. инетерфейсов

};

FILE *fp;
struct info info;
struct sys_info sys_info;

void openFunc(char name[]){
	if ((fp = fopen(name, "r")) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

void version_info() {
    char ver[] = "/proc/version";
    openFunc(ver);

    fgets(sys_info.version, N+5, fp);
    fclose(fp);
}

void kernel_info() {
    fgets(sys_info.version, N+5, fp);
    fclose(fp);
}

void network_interaces() {
    /////////////////INTERFACES/////////////////////
	system("rm ./tmp.txt && ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFunc(name0);

        // очистка массива для перезаписи
    memset(sys_info.net_int[N], 0, sizeof(sys_info.net_int[N]));    
    
	while (!feof(fp)){
		fscanf(fp, "%s", sys_info.net_int[sys_info.count]);
		sys_info.count++;
	}
    sys_info.count -= 1;    // исключает ошибку feof - дублирует последнюю строку
	fclose(fp);
}

// ##########################################

void memory_info() {
    //////////////MEMORY////////////////////////////
	char name1[] = "/proc/meminfo";
	openFunc(name1);
	
	fgets(info.mem.memTotal, N, fp);
	fgets(info.mem.memFree, N, fp);
	fgets(info.mem.memAvail, N, fp);
	fclose(fp);
}

void cpu_info() {
    /////////////////CPU////////////////////////////
	char name2[] = "/proc/loadavg";
	openFunc(name2);

	fgets(info.cpuavg, N, fp);
	fclose(fp);
}

// ##########################################

void get_info() {
    version_info();
    kernel_info();
    network_interaces();
    
    memory_info();
    cpu_info();
}


void out(){
    printf("Version: %s\n", sys_info.version);

    printf("Net Int.:        ");
	for(int i=0; i < sys_info.count; i++){	
		printf("%s ", sys_info.net_int[i]);
	}

    printf("\n-------------------------------\n");

	printf("%s", info.mem.memTotal);
	printf("%s", info.mem.memFree);
	printf("%s", info.mem.memAvail);

	printf("CPU:\t%s", info.cpuavg);

    printf("\n-------------------------------\n");

}

int main(void){
    printf("\n\tSystem information\n\n");

    get_info();
    out();

    return 0;
}