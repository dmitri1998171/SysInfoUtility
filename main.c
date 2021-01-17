#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define arr_size 50

struct mem {
	int memTotal;	// Полный объем
	int memAvail;	// Используется
	int swapTotal;	// swap полный
	int swapAvail;	// swap исп.
};

struct info {
	char cpuavg[15];	  // Нагрузка процессора
	struct mem mem;		  // ОЗУ и swap
}info;

struct sys_info {
    char version[arr_size];      // Версия Линукс
    char kernel[arr_size];       // Ядро
	char net_int[arr_size][arr_size];	  // Сетевые интерфейсы
	unsigned int count;	  // счетчик кол-ва сетев. инетерфейсов
	char cpu[arr_size*14];

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

    fgets(sys_info.version, arr_size+5, fp);
    fclose(fp);
}

void kernel_info() {
    fgets(sys_info.version, arr_size, fp);
    fclose(fp);
}

void network_interaces() {
	system("rm ./tmp.txt && ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFile(name0, 'r');

        // очистка массива для перезаписи
    memset(sys_info.net_int[arr_size], 0, sizeof(sys_info.net_int[arr_size]));    
    
	while (!feof(fp)){
		fscanf(fp, "%s", sys_info.net_int[sys_info.count]);
		sys_info.count++;
	}
    sys_info.count -= 1;    // исключает ошибку feof - дублирует последнюю строку
	fclose(fp);
}

// ##########################################

char* parsing(char *str){
	char *p = strtok(str," ");
	p = strtok(NULL, " ");
	return p;
}

void get_number_from_str(char* str, int* value) {
	*value = atoi(str);
	*value = *value / 1024; // перевод Kb->Mb
}

void mem_info(){
    char name1[] = "/proc/meminfo";
	char str[arr_size];

    FILE *f = fopen(name1, "r");
    while (fgets(str, arr_size, f)) {
        if(strstr(str, "MemTotal"))
			get_number_from_str(parsing(str), &info.mem.memTotal);
        if(strstr(str, "MemAvailable"))
			get_number_from_str(parsing(str), &info.mem.memAvail);
        if(strstr(str, "SwapTotal"))
			get_number_from_str(parsing(str), &info.mem.swapTotal);
        if(strstr(str, "SwapFree")) {
			int tmp = 0;
			get_number_from_str(parsing(str), &tmp);
			info.mem.swapAvail = info.mem.swapTotal - tmp;
		}
    }
    fclose(f);
}

void cpu_info() {
	char name2[] = "/proc/loadavg";
	openFile(name2, 'r');

	fgets(info.cpuavg, 15, fp);
	fclose(fp);

	char name3[] = "/proc/cpuinfo";
	char str[arr_size*4];
	openFile(name3, 'r');

	while(!feof(fp)) {
		fgets(str, arr_size*4, fp);

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
    
	mem_info();
    cpu_info();
}

void current_values_output() {
	printf("CPU avg: %s\n", info.cpuavg);
	printf("GPU:\n");
	printf("RAM: %i / %i Mb\n", info.mem.memAvail, info.mem.memTotal);
	printf("Swap: %i / %i Mb\n", info.mem.swapAvail, info.mem.swapTotal);
}

void out(){
    printf("Version: %s\n", sys_info.version);

    printf("Network interfaces: ");
	for(int i=0; i < sys_info.count; i++){	
		printf("%s ", sys_info.net_int[i]);
	}

	printf("\nCPU: %s", sys_info.cpu);
	printf("GPU: \n");
	printf("RAM: %i Mb\n", info.mem.memTotal);
	printf("Swap: %i Mb\n", info.mem.swapTotal);

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

	fprintf(fp, "RAM: %i / %i Mb\n", info.mem.memAvail, info.mem.memTotal);
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