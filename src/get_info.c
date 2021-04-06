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
				get_number_from_str(parsing(str, " ", 1), &mem.memTotal);
			if(strstr(str, "MemAvailable"))
				get_number_from_str(parsing(str, " ", 1), &mem.memAvail);
			if(strstr(str, "SwapTotal"))
				get_number_from_str(parsing(str, " ", 1), &mem.swapTotal);
			if(strstr(str, "SwapFree")) {
				int tmp = 0;
				get_number_from_str(parsing(str, " ", 1), &tmp);
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
			if(value = strstr(str, "graphics memory")) {
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
		fgets(str, ARR_SIZE, fp);
		char *ptr = strtok(str, "Linux version ");
		strcpy(hard_info.version, ptr);
		fclose(fp);
	}
}

void network_interaces() {
	struct dirent *dir;
    
	DIR *d = opendir("/sys/class/net");
	if(d != NULL) { 
		while((dir = readdir(d))){
			if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) continue;

			strcpy(hard_info.net_int[hard_info.net_int_count], dir->d_name);
			hard_info.net_int_count++;
		}
		closedir(d);
	}
}

void cpu_hard_info() {
	char str[ARR_SIZE];

	if ((fp = fopen("/proc/cpuinfo", "r")) != NULL) {
		while(fgets(str, ARR_SIZE, fp)) {
			if(strstr(str, "model name")) {
				char *d = parsing(str, ":", 1);
				strcpy(hard_info.cpu, d);
			}
			if(strstr(str, "cpu cores")) {
				char *p = parsing(str, ":", 1);
				hard_info.cpu_cores = atoi(p);
				break;
			}
		}
		fclose(fp);
	}
}

void resolution() {
	char str[12];

	if ((fp = fopen("/sys/class/graphics/fb0/virtual_size", "r")) != NULL) {
		fgets(str, 12, fp);
		strncpy(hard_info.resolution, str, strlen(str)-1);
		fclose(fp);
	}
}

void hdd_ssd_info() {
    if ((fp = fopen("/proc/partitions", "r")) != NULL) {
		char *p;
		char str[ARR_SIZE];
		volumes_info.partitions_count = 0;
		volumes_info.volumes_count = 0;
		
		while (fgets(str, ARR_SIZE, fp)) {
			if(p = strstr(str, "sd")) {
				if(strlen(p) == 4) {
					strncpy(volumes_info.volumes[volumes_info.volumes_count], p, strlen(p) - 1);
					volumes_info.vol_size[volumes_info.volumes_count] = atof(parsing(str, " ", 2)) / 1024;
					volumes_info.volumes_count++;
				}
				else {
					strncpy(volumes_info.partitions[volumes_info.partitions_count], p, strlen(p) - 1);
					volumes_info.part_size[volumes_info.partitions_count] = atof(parsing(str, " ", 2)) / 1024;
					volumes_info.partitions_count++;
				}
			}
		}
		fclose(fp);
	}
}

void get_hard_info() {
    version_info();
    network_interaces();
	cpu_hard_info();
	resolution();
	mem_info();
	hdd_ssd_info();
}

