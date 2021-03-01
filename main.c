#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define ARR_SIZE 50

static struct option long_opt[] = {
				{"hard_info", 0, 0, 'w'},
				{"sys_info", 0, 0, 's'},
				{"log", 0, 0, 'l'},
				{"html", 0, 0, 'h'},
				{"help", 0, 0, '?'},
				{0,0,0,0}
};

struct mem {
	int memTotal;	// Полный объем
	int memAvail;	// Используется
	int swapTotal;	// swap полный
	int swapAvail;	// swap исп.
};

struct sys_info {
	float cpu_load;	  	  // Нагрузка процессора
	char cpuavg[15];	  // Нагрузка процессора
	int gpuavg;			  // Объем видеопамяти
	struct mem mem;		  // ОЗУ и swap
}sys_info;

struct hard_info {
    char version[ARR_SIZE];      	  // Версия ядра линукс
	char net_int[ARR_SIZE][ARR_SIZE]; // Сетевые интерфейсы
	char cpu[ARR_SIZE];			 	  // Процессор
	int cpu_cores;		 	 		  // Кол-во ядер
	unsigned int count;	  			  // кол-во сетев. инетерфейсов
}hard_info;

struct graph_strings {
	char string_name[6][15];		  // Имена строк для граф. режимов
	float string_load[6];				  // Загруженность 
}graph_strings;

FILE *fp;

void graph_strings_Func() {
	strcpy(graph_strings.string_name[0], "HDD/SSD load");
	strcpy(graph_strings.string_name[1], "CPU t*C");
	strcpy(graph_strings.string_name[2], "GPU t*C");
	strcpy(graph_strings.string_name[3], "CPU avg");
	strcpy(graph_strings.string_name[4], "GPU load");
	strcpy(graph_strings.string_name[5], "Memory load");

	// graph_strings.string_load[0] = 
	// graph_strings.string_load[0] = 
	// graph_strings.string_load[0] = 
	graph_strings.string_load[3] = (sys_info.cpu_load / hard_info.cpu_cores) * 100;
	// graph_strings.string_load[0] = 
	graph_strings.string_load[5] = sys_info.gpuavg / 10;
}

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
	*value = *value / 1000; // перевод Kb->Mb			??? 1000 или 1024
	// https://habr.com/ru/post/193256/
}

// ##### SYSTEM INFO #######################

void cpu_sys_info() {
	char name2[] = "/proc/loadavg";
	char str[15];

	openFile(name2, 'r');
	fgets(str, 15, fp);
	fclose(fp);

	strcpy(sys_info.cpuavg, str);
	char *ptr = strtok(str, " ");
     sys_info.cpu_load = atof(ptr);
}

void mem_info() {
    char name1[] = "/proc/meminfo";
	char str[ARR_SIZE];

    openFile(name1, 'r');
    while (fgets(str, ARR_SIZE, fp)) {
        if(strstr(str, "MemTotal"))
			get_number_from_str(parsing(str, " "), &sys_info.mem.memTotal);
        if(strstr(str, "MemAvailable"))
			get_number_from_str(parsing(str, " "), &sys_info.mem.memAvail);
        if(strstr(str, "SwapTotal"))
			get_number_from_str(parsing(str, " "), &sys_info.mem.swapTotal);
        if(strstr(str, "SwapFree")) {
			int tmp = 0;
			get_number_from_str(parsing(str, " "), &tmp);
			sys_info.mem.swapAvail = sys_info.mem.swapTotal - tmp;
		}
    }
    fclose(fp);
}

void gpu_sys_info() {
	char str[73];	

	FILE* f = popen("dmesg | grep \'graphics memory\'", "r");
    if (f) 
		fgets(str, 73, f);
	pclose(f);
    	
	char *value = strstr(str, "memory: ");
	value = strstr(value, " ");
	value = strtok(value, " ");

	sys_info.gpuavg = atoi(value) / 1000 / 13.9;	// ???
	// https://habr.com/ru/post/193256/
}

void get_sys_info() {
    cpu_sys_info();
	mem_info();
	gpu_sys_info();
}

// ##### HARD INFO ########################

void version_info() {
    char ver[] = "/proc/version";
    openFile(ver, 'r');
    fgets(hard_info.version, ARR_SIZE-14, fp);
    fclose(fp);
}

void network_interaces() {
	system("ls /sys/class/net >> ./tmp.txt");

	char name0[] = "tmp.txt";
	openFile(name0, 'r');

        // очистка массива для перезаписи
    memset(hard_info.net_int[ARR_SIZE], 0, ARR_SIZE);    
    
	while (!feof(fp)){
		fscanf(fp, "%s", hard_info.net_int[hard_info.count]);
		hard_info.count++;
	}
	// исключает ошибку feof - дублирует последнюю строку
    hard_info.count -= 1;
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
			strcpy(hard_info.cpu, d);
		}
		if(strstr(str, "cpu cores")) {
			char *p = parsing(str, ":");
			hard_info.cpu_cores = atoi(p);
			break;
		}
	}
	fclose(fp);
}

void get_hard_info() {
    version_info();
    network_interaces();
	cpu_hard_info();
	mem_info();
}

// ##### OUTPUT #############################

void current_values_output() {
	printf("CPU avg: %s\n", sys_info.cpuavg);
	printf("GPU: %i Mb\n", sys_info.gpuavg);
	printf("RAM: %i / %i Mb\n", sys_info.mem.memAvail, sys_info.mem.memTotal);
	printf("Swap: %i / %i Mb\n", sys_info.mem.swapAvail, sys_info.mem.swapTotal);
}

void out() {
    printf("Version: %s\n", hard_info.version);
    printf("Network interfaces: ");
	
	for(int i=0; i < hard_info.count; i++)	
		printf("%s ", hard_info.net_int[i]);
	
	printf("\nCPU: %s\n", hard_info.cpu);
	printf("CPU CORES: %i\n", hard_info.cpu_cores);
	printf("GPU: \n");
	printf("RAM: %i Mb\n", sys_info.mem.memTotal);
	printf("Swap: %i Mb\n", sys_info.mem.swapTotal);
}

void write_to_log() {
	char filename[] = {"sysInfo.log"};
	openFile(filename, 'w');

	fprintf(fp, "Version: %s\n", hard_info.version);
    fprintf(fp, "Network interfaces: ");
	
	for(int i = 0; i < hard_info.count; i++)	
		fprintf(fp, "%s ", hard_info.net_int[i]);

	fprintf(fp, "\nCPU:\t%s\n", hard_info.cpu);
	fprintf(fp, "CPU CORES:\t%i\n", hard_info.cpu_cores);
	fprintf(fp, "GPU: \n");
	fprintf(fp, "RAM: %i Mb\n", sys_info.mem.memTotal);
	fprintf(fp, "Swap: %i Mb\n", sys_info.mem.swapTotal);

	fprintf(fp, "CPU avg: %s\n", sys_info.cpuavg);
	fprintf(fp, "GPU:\n");
	fprintf(fp, "RAM: %i / %i Mb\n", sys_info.mem.memAvail, sys_info.mem.memTotal);
	fprintf(fp, "Swap: %i / %i Mb\n", sys_info.mem.swapAvail, sys_info.mem.swapTotal);

	fclose(fp);
}

void generate_html() {
	char str[ARR_SIZE];
    FILE *fsrc, *fdst;

	graph_strings_Func();

    if ((fsrc = fopen("sysInfo.log", "r")) == NULL) {
		perror("fopen"); exit(-1); }
    if ((fdst = fopen("logfile.html", "w")) == NULL) {
		perror("fopen"); exit(-1); }

    fprintf(fdst, "<html>\n");
	fprintf(fdst, "\t<head>\n");
	fprintf(fdst, "\t\t<title>logfile.html</title>\n");
	fprintf(fdst, "\t\t<link rel=\"stylesheet\" href=\"style.css\"></link>\n");
	fprintf(fdst, "\t</head>\n");
    fprintf(fdst, "\t<body>\n");
	fprintf(fdst, "\t\t<div class=\"bg_block\">\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");
	fprintf(fdst, "\t\t\t\t<div class=\"subblock\" style=\"padding-bottom: 14px;\">\n");
	
    while(fgets(str, ARR_SIZE, fsrc)) {
		if(strstr(str, "CPU avg:")) { // Отделение текущ. инфы в отдельный блок
			fprintf(fdst, "\t\t\t\t</div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\"><br></div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		}
        fprintf(fdst, "\t\t\t\t\t%s<br>", str);
    }
    fprintf(fdst, "\n\t\t\t\t</div>\n");
    fprintf(fdst, "\t\t\t</div>\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");

	for(int i = 0; i < 6; i++) {
		if(i == 3)
        	fprintf(fdst, "<div class=\"subblock\"><br></div>\n");

		fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		fprintf(fdst, "\t\t\t\t<table  width=\"100%\">\n");
		fprintf(fdst, "\t\t\t\t\t<tr>\n");

		fprintf(fdst, "\t\t\t\t\t<td class=\"td_line\" width=\"80%\"> <hr style=\"width: %f%;\"> </td>\n", graph_strings.string_load[i]);
		fprintf(fdst, "\t\t\t\t\t<td align=\"center\"> %s </td>\n", graph_strings.string_name[i]);
		
		fprintf(fdst, "\t\t\t\t\t</tr>\n");
		fprintf(fdst, "\t\t\t\t</table>\n");
		fprintf(fdst, "\t\t\t\t</div>\n");
	}
    fprintf(fdst, "\t\t\t</div>\n");
    fprintf(fdst, "\t\t</div>\n");
    fprintf(fdst, "\t</body>\n");
    fprintf(fdst, "</html>");

    fclose(fsrc);
    fclose(fdst);
}

void full_output() {
	get_hard_info();			// сбор хар-к ПК
	get_sys_info();				// сбор текущих значений
    
	printf("\n\tSystem information\n\n");
	out();						// Вывод хар-к ПК
	printf("\n-------------------------------\n\n");
	current_values_output();	// Вывод текущих значений
}

int main(int argc, char *argv[]) {
	if(argc == 1)
		full_output();
	else {
		int res, optIdx;

		while ((res = getopt_long(argc,argv,"wslh", long_opt, &optIdx)) != -1) {
			switch(res) {
				case 'w': { get_hard_info(); out(); break; }	// Вывод хар-к ПК
				case 's': { get_sys_info(); current_values_output(); break; }	// Вывод текущ. знач.
				case 'l': { full_output(); write_to_log(); break; }	// запись в лог
				case 'h': { full_output(); write_to_log(); generate_html(); break; }	// запись в html
				
				case '?': { printf("\n Usage: %s [OPTION]\n\n -w, --hard_info print hardware information\n -s, --sys_info  print system information\n -l, --log       output to \'sysInfo.log\' file\n -h, --html\t output to html\n -?, --help\t print this help\n\n By default, without options, the utility displays hardware and system information\n\n", argv[0]); break; }
			}
		}
	}

    return 0;
}