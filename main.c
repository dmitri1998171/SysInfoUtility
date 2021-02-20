#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define ARR_SIZE 50

struct mem {
	int memTotal;	// Полный объем
	int memAvail;	// Используется
	int swapTotal;	// swap полный
	int swapAvail;	// swap исп.
};

struct info {
	char cpuavg[15];	  // Нагрузка процессора
	int gpuavg;			  // Объем видеопамяти
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
	*value = *value / 1000; // перевод Kb->Mb			??? 1000 или 1024
	// https://habr.com/ru/post/193256/
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

void gpu_sys_info() {
	char str[73];	
	FILE* f;

    f = popen("dmesg | grep \'graphics memory\'", "r");
    if (f) 
		fgets(str, 73, f);
	pclose(f);
    	
	char *value = strstr(str, "memory: ");
	value = strstr(value, " ");
	value = strtok(value, " ");

	info.gpuavg = atoi(value) / 1024;
}
 
void get_sys_info() {
    cpu_sys_info();
	mem_info();
	gpu_sys_info();
}

// ##### OUTPUT #############################

void current_values_output() {
	printf("CPU avg: %s\n", info.cpuavg);
	printf("GPU: %i Mb\n", info.gpuavg);
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

	fprintf(fp, "Version: %s\n", sys_info.version);
    fprintf(fp, "Network interfaces: ");
	
	for(int i=0; i < sys_info.count; i++)	
		fprintf(fp, "%s ", sys_info.net_int[i]);

	fprintf(fp, "\nCPU:\t%s\n", sys_info.cpu);
	fprintf(fp, "GPU: \n");
	fprintf(fp, "RAM: %i Mb\n", info.mem.memTotal);
	fprintf(fp, "Swap: %i Mb\n", info.mem.swapTotal);

	fprintf(fp, "CPU avg: %s\n", info.cpuavg);
	fprintf(fp, "GPU:\n");
	fprintf(fp, "RAM: %i / %i Mb\n", info.mem.memAvail, info.mem.memTotal);
	fprintf(fp, "Swap: %i / %i Mb\n", info.mem.swapAvail, info.mem.swapTotal);

	fclose(fp);
}

void generate_html() {
	char str[ARR_SIZE];

    FILE *fsrc, *fdst;
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
	
    while(fgets(str, ARR_SIZE+10, fsrc)) {
		if(strstr(str, "CPU avg:")){
			fprintf(fdst, "\t\t\t\t</div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\"><br></div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		}
        fprintf(fdst, "\t\t\t\t\t<br>%s", str);
    }

    fprintf(fdst, "\t\t\t\t</div>\n");
    fprintf(fdst, "\t\t\t</div>\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");

//	shedule

    fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
    fprintf(fdst,  "\t\t\t\t<table  width=\"100%\">\n");
    fprintf(fdst,   "\t\t\t\t\t<tr> \n");
    fprintf(fdst,    "\t\t\t\t\t<td> <hr> </td>\n");

// строка с изменяемыми параметрами
    fprintf(fdst,     "\t\t\t\t\t<td width=\"25%\" align=\"center\"> HDD/SSD load </td>\n");
    
	fprintf(fdst,      "\t\t\t\t\t</tr>\n");
    fprintf(fdst,     "\t\t\t\t</table>\n");
    fprintf(fdst,  "\t\t\t\t</div>\n");

//

    fprintf(fdst,  "\t\t\t</div>\n");
    fprintf(fdst,  "\t\t</div>\n");


	    /*
        <div class=\"subblock\">
            <table  width=\"100%\">
                <tr> 
                    <td> <hr> </td>
                    <td width=\"25%\" align=\"center\"> CPU t*C </td>
                </tr>
            </table>
        </div>

        <div class="subblock">
            <table  width=\"100%\">
                <tr> 
                    <td> <hr> </td>
                    <td width=\"25%\" align=\"center\"> GPU t*C </td>
                </tr>
            </table>
        </div>

        <div class=\"subblock\"><br></div>

        <div class=\"subblock\">
            <table  width=\"100%\">
                <tr> 
                    <td> <hr> </td>
                    <td width=\"25%\" align=\"center\"> CPU avg. </td>
                </tr>
            </table>
        </div>

        <div class=\"subblock\">
            <table  width=\"100%\">
                <tr> 
                    <td> <hr> </td>
                    <td width=\"25%\" align=\"center\"> GPU </td>
                </tr>
            </table>
        </div>

        <div class=\"subblock\">
            <table  width=\"100%\">
                <tr> 
                    <td> <hr> </td>
                    <td width=\"25%\" align=\"center\"> Memory </td>
                </tr>
            </table>
        </div> 
    </div>");
*/

    fprintf(fdst, "\t</body>\n");
    fprintf(fdst, "</html>");

    fclose(fsrc);
    fclose(fdst);
}

void full_output() {
	get_hard_info();			// сбор хар-к ПК
	get_sys_info();				// сбор текущих значений
	out();						// Вывод хар-к ПК
	printf("\n-------------------------------\n\n");
	current_values_output();	// Вывод текущих значений
}

int main(int argc, char *argv[]) {
	if(argc == 1)
		full_output();
	
	int res = 0;
	while ((res = getopt(argc,argv,"wslh")) != -1) {
		switch(res) {
			case 'w': { get_hard_info(); out(); break; }	// Вывод хар-к ПК
			case 's': { get_sys_info(); current_values_output(); break; }	// Вывод текущ. знач.
			case 'l': { full_output(); write_to_log(); break; }	// запись в лог
			case 'h': { full_output(); write_to_log(); generate_html(); break; }	// запись в html
			
			case '?': { printf("\n Usage: %s [OPTION]\n\n -w, --hardware  print hardware information\n -s, --system    print system information\n -l, --log       output to \'sysInfo.log\' file\n -h, --html\t output to html\n -?\t\t print this help\n\n By default, without options, the utility displays hardware and system information\n\n", argv[0]); break; }
        }
	}

    return 0;
}