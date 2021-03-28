#include "../include/protocol.h"   

// ##### SYSTEM INFO #######################

void cpu_sys_info() {
	char str[15];

	if ((fp = fopen("/proc/loadavg", "r")) != NULL) {
		fgets(str, 15, fp);
		strcpy(sys_info.cpuavg, str);
		char *ptr = strtok(str, " ");
		sys_info.cpu_load = atof(ptr);
		fclose(fp);
	}
}

void mem_info() {
	char str[ARR_SIZE];

    if ((fp = fopen("/proc/meminfo", "r")) != NULL) {
		while (fgets(str, ARR_SIZE, fp)) {
			if(strstr(str, "MemTotal"))
				get_number_from_str(parsing(str, " "), &mem.memTotal);
			if(strstr(str, "MemAvailable"))
				get_number_from_str(parsing(str, " "), &mem.memAvail);
			if(strstr(str, "SwapTotal"))
				get_number_from_str(parsing(str, " "), &mem.swapTotal);
			if(strstr(str, "SwapFree")) {
				int tmp = 0;
				get_number_from_str(parsing(str, " "), &tmp);
				mem.swapAvail = mem.swapTotal - tmp;
			}
		}
		fclose(fp);
	}
}

void gpu_sys_info() {
	char *value;
	char str[ARR_SIZE];

	if ((fp = fopen("/var/log/syslog.1", "r")) != NULL) {
		while (fgets(str, ARR_SIZE, fp)) {
			if(value = strstr(str, "graphics memory: ")) {
				value = strstr(value, " ");
				value = strtok(value, " ");
				sys_info.gpuavg = atoi(value) / 1000 / 13.9;
			}
		}
    	fclose(fp);
	}
}

void cpu_temp_info() {
	char str[15];

	if((fp = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon5/temp1_input", "r")) != NULL) {
		fgets(str, 15, fp);

	//	strcpy(sys_info.cpuavg, str);
	//	char *ptr = strtok(str, " ");
		sys_info.cpu_temp_avg = atoi(str) / 1000;
		fclose(fp);
	}
}

void get_sys_info() {
    cpu_sys_info();
	mem_info();
	gpu_sys_info();
	cpu_temp_info();
}

// ##### HARD INFO ########################

void version_info() {
	char str[ARR_SIZE];

    if ((fp = fopen("/proc/version", "r")) != NULL) {
		fgets(str, ARR_SIZE-14, fp);
		char *ptr = strtok(str, "Linux version ");
		strcpy(hard_info.version, ptr);
		fclose(fp);
	}
}

void network_interaces() {
	system("ls /sys/class/net >> ./tmp.txt");

	if ((fp = fopen("tmp.txt", "r")) != NULL) {
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
}

void cpu_hard_info() {
	char str[ARR_SIZE];

	if ((fp = fopen("/proc/cpuinfo", "r")) != NULL) {
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
}

void resolution() {
	if ((fp = fopen("/sys/class/graphics/fb0/virtual_size", "r")) != NULL) {
		fgets(hard_info.resolution, ARR_SIZE/4, fp);
		fclose(fp);
	}
}

void get_hard_info() {
    version_info();
    network_interaces();
	cpu_hard_info();
	resolution();
	mem_info();
}

