#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ARR_SIZE 50

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
    char version[ARR_SIZE];      // Версия ядра линукс
	char net_int[ARR_SIZE][ARR_SIZE]; // Сетевые интерфейсы
	char cpu[ARR_SIZE];			 		 // Процессор
	int cpu_cores;		 	 // Кол-во ядер
	unsigned int count;	  // счетчик кол-ва сетев. инетерфейсов
}sys_info;

FILE *fp;

void openFile(char name[], char attr) {
	if ((fp = fopen(name, &attr)) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

char* parsing(char *str, char *symbol) {
	char *p = strtok(str, symbol);
	p = strtok(NULL, symbol);
	return p;
}

void get_number_from_str(char* str, int* value) {
	*value = atoi(str);
	*value = *value / 1024; // перевод Kb->Mb
}

// ##### HARD INFO ########################

void version_info() {
    char ver[] = "/proc/version";
    openFile(ver, 'r');
    fgets(sys_info.version, ARR_SIZE, fp);
    fclose(fp);
}

void network_interaces() {
	system("ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFile(name0, 'r');

        // очистка массива для перезаписи
    memset(sys_info.net_int[ARR_SIZE], 0, ARR_SIZE);    
    
	while (!feof(fp)){
		fscanf(fp, "%s", sys_info.net_int[sys_info.count]);
		sys_info.count++;
	}
	// исключает ошибку feof - дублирует последнюю строку
    sys_info.count -= 1;
	fclose(fp);
	system("rm ./tmp.txt");
}

void cpu_hard_info() {
	char name3[] = "/proc/cpuinfo";
	char str[ARR_SIZE];

	openFile(name3, 'r');
	while(fgets(str, ARR_SIZE, fp)) {
		if(strstr(str, "model name")) {
			char *d = parsing(str, ":");
			strcpy(sys_info.cpu, d);
		}
		if(strstr(str, "cpu cores")) {
			char *p = parsing(str, ":");
			sys_info.cpu_cores = atoi(p);
			break;
		}
	}
	fclose(fp);
}

void get_hard_info() {
    version_info();
    network_interaces();
	cpu_hard_info();
}

// ##### SYSTEM INFO #######################

void cpu_sys_info() {
	char name2[] = "/proc/loadavg";
	openFile(name2, 'r');
	fgets(info.cpuavg, 15, fp);
	fclose(fp);
}

void mem_info() {
    char name1[] = "/proc/meminfo";
	char str[ARR_SIZE];

    openFile(name1, 'r');
    while (fgets(str, ARR_SIZE, fp)) {
        if(strstr(str, "MemTotal"))
			get_number_from_str(parsing(str, " "), &info.mem.memTotal);
        if(strstr(str, "MemAvailable"))
			get_number_from_str(parsing(str, " "), &info.mem.memAvail);
        if(strstr(str, "SwapTotal"))
			get_number_from_str(parsing(str, " "), &info.mem.swapTotal);
        if(strstr(str, "SwapFree")) {
			int tmp = 0;
			get_number_from_str(parsing(str, " "), &tmp);
			info.mem.swapAvail = info.mem.swapTotal - tmp;
		}
    }
    fclose(fp);
}

void get_sys_info() {
    cpu_sys_info();
	mem_info();
}

// ##### OUTPUT #############################

void current_values_output() {
	printf("CPU avg: %s\n", info.cpuavg);
	printf("GPU:\n");
	printf("RAM: %i / %i Mb\n", info.mem.memAvail, info.mem.memTotal);
	printf("Swap: %i / %i Mb\n", info.mem.swapAvail, info.mem.swapTotal);
}

void out() {
    printf("\n\tSystem information\n\n");
    printf("Version: %s\n", sys_info.version);
    printf("Network interfaces: ");
	
	for(int i=0; i < sys_info.count; i++)	
		printf("%s ", sys_info.net_int[i]);
	
	printf("\nCPU: %s\n", sys_info.cpu);
	printf("CPU CORES: %i\n", sys_info.cpu_cores);
	printf("GPU: \n");
	printf("RAM: %i Mb\n", info.mem.memTotal);
	printf("Swap: %i Mb\n", info.mem.swapTotal);
}

void write_to_log() {
	char filename[] = {"sysInfo.log"};
	openFile(filename, 'w');

    fprintf(fp, "\n\tSystem information\n\n");
	fprintf(fp, "Version: %s\n", sys_info.version);
    fprintf(fp, "Network interfaces: ");
	
	for(int i=0; i < sys_info.count; i++)	
		fprintf(fp, "%s ", sys_info.net_int[i]);

	fprintf(fp, "\nCPU:\t%s", sys_info.cpu);
	fprintf(fp, "GPU: \n");
	fprintf(fp, "RAM: %i Mb\n", info.mem.memTotal);
	fprintf(fp, "Swap: %i Mb\n", info.mem.swapTotal);
    fprintf(fp, "\n-------------------------------\n\n");

	fprintf(fp, "CPU avg: %s\n", info.cpuavg);
	fprintf(fp, "GPU:\n");
	fprintf(fp, "RAM: %i / %i Mb\n", info.mem.memAvail, info.mem.memTotal);
	fprintf(fp, "Swap: %i / %i Mb\n", info.mem.swapAvail, info.mem.swapTotal);

	fclose(fp);
}

int main(int argc, char *argv[]) {
	if(argc == 1){
		get_hard_info();			// сбор хар-к ПК
		get_sys_info();				// сбор текущих значений
		out();						// Вывод хар-к ПК
		printf("\n-------------------------------\n\n");
		current_values_output();	// Вывод текущих значений
	}

	int res = 0;
	while ((res = getopt(argc,argv,"wsl")) != -1) {
		switch(res) {
			case 'w': { get_hard_info(); out(); break; }	// Вывод хар-к ПК
			case 's': { get_sys_info(); current_values_output(); break; }	// Вывод текущ. знач.
			case 'l': { write_to_log(); break; }	// запись в лог
			case '?':
			case 'h': { printf("\n Usage: %s [OPTION]\n\n -w, --hardware  print hardware information\n -s, --system    print system information\n -l, --log       output in \'sysInfo.log\' file\n -h, --help      print this help\n\n By default, without options, the utility displays hardware and system information\n\n", argv[0]); break; }
        }
	}

    return 0;
}