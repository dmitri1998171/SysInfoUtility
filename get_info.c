#include "protocol.h"   

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

