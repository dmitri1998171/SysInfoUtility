#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define N 50

struct mem {
	char memTotal[N];	// ОЗУ
	char memAvail[N];	//
};

struct info {
	char cpuavg[15];		  // Нагрузка процессора
	struct mem mem;		  // ОЗУ
}info;

struct sys_info {
    char version[N];      // Версия Линукс
    char kernel[N];       // Ядро
	char net_int[N][N];	  // Сетевые интерфейсы
	unsigned int count;	  // счетчик кол-ва сетев. инетерфейсов
	char cpu[N*14];

}sys_info;

FILE *fp;

void openFile(char name[], char attr){
	if ((fp = fopen(name, &attr)) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

void version_info() {
    char ver[] = "/proc/version";
    openFile(ver, 'r');

    fgets(sys_info.version, N+5, fp);
    fclose(fp);
}

void kernel_info() {
    fgets(sys_info.version, N, fp);
    fclose(fp);
}

void network_interaces() {
	system("rm ./tmp.txt && ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFile(name0, 'r');

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
	char name1[] = "/proc/meminfo";
	char str[N];
	openFile(name1, 'r');
	
	fgets(str, N, fp);	// Total
	char *p = strtok(str," ");
	p = strtok(NULL, " ");
	strcpy(info.mem.memTotal, p);

	fgets(str, N, fp);	// Free

	fgets(str, N, fp);	// Available
	char *d = strtok(str," ");
	d = strtok(NULL, " ");
	strcpy(info.mem.memAvail, p);

	fclose(fp);
}

void cpu_info() {
	char name2[] = "/proc/loadavg";
	openFile(name2, 'r');

	fgets(info.cpuavg, 15, fp);
	fclose(fp);

	char name3[] = "/proc/cpuinfo";
	char str[N*4];
	openFile(name3, 'r');

	while(!feof(fp)) {
		fgets(str, N*4, fp);

		if(strstr(str, "model name") != NULL ){
			char *p = strtok(str, ":");
			p = strtok(NULL, ":");
			strcpy(sys_info.cpu, p);
			break;
		}
	}
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


void current_values_output() {
	printf("CPU avg: %s\n", info.cpuavg);
	printf("GPU:\n");
	printf("RAM: %s / %s\n", info.mem.memAvail, info.mem.memTotal);
}

void out(){
    printf("Version: %s\n", sys_info.version);

    printf("Network interfaces: ");
	for(int i=0; i < sys_info.count; i++){	
		printf("%s ", sys_info.net_int[i]);
	}

	printf("\nCPU: %s", sys_info.cpu);
	printf("GPU: \n");
	printf("RAM: %s\n", info.mem.memTotal);

    printf("\n-------------------------------\n");

	current_values_output();

}

void write_to_log() {
	char filename[] = {"sysInfo.log"};

	openFile(filename, 'w');
	fprintf(fp, "Version: %s\n", sys_info.version);

    fprintf(fp, "Net Int.:        ");
	for(int i=0; i < sys_info.count; i++){	
		fprintf(fp, "%s ", sys_info.net_int[i]);
	}

    fprintf(fp, "\n-------------------------------\n");

	fprintf(fp, "%s", info.mem.memTotal);
	fprintf(fp, "%s", info.mem.memAvail);

	fprintf(fp, "CPU:\t%s", info.cpuavg);

    fprintf(fp, "\n-------------------------------\n");

	fclose(fp);
}

int main(void){
    printf("\n\tSystem information\n\n");

    get_info();			// сбор инфы
    out();				// вывод в консоль
	write_to_log();		// запись в лог

    return 0;
}