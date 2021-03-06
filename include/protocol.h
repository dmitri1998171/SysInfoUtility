#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h> 
#include <mntent.h>
#include <sys/statvfs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>

#define ARR_SIZE 55

FILE *fp;
pthread_mutex_t mutex, ncurses_mutex;
struct ThreadArgs *threadArgs;

struct ThreadArgs{
    int clntSock;
};

struct mem {
	int mem_total;					  // Полный объем
	int mem_used;					  // Используется
	int swap_total;				  // swap полный
	int swap_used;					  // swap исп.
}mem;

struct volumes_info {
	int volumes_count;				  // Кол-во дисков hdd/ssd
	int partitions_count;			  	  // Кол-во разделов
	float vol_size[4];				  // Обьем hdd/ssd в MB
	float vol_free[4];				  // Объем свободного места на диске в MB
	float part_free[4];				  // ... в разделе
	float part_size[4];				  // Обьем разделов в MB
	char volumes[4][5];			  	  // Физ. диски hdd/ssd
	char partitions[4][5];			  	  // разделы диска
}volumes_info;

struct sys_info {
	int gpu_total;			   		  // Общий объем видеопамяти
	int gpu_used;			   		  // Используемый объем 
	int cpu_temp_avg;	   			  // Cpu temp 
	int cpu_temp_max;				  // ... max
	int gpu_temp_avg;				  // Gpu temp 
	int gpu_temp_max;				  // ... max
	char cpu[15];					  // Нагрузка процессора (для строк)
	float cpu_load_avg;	  	   		  // Нагрузка процессора средняя (для формул)
}sys_info;

struct hard_info {
	unsigned int cpu_cores;		 	  // Кол-во ядер
	unsigned int net_int_count;	  		  // Кол-во сетев. инетерфейсов
	char resolution[10];				  // Разрешение экрана
    	char version[ARR_SIZE];      	  		  // Версия ядра линукс
	char cpu[ARR_SIZE];			 	  // Процессор
	char gpu[ARR_SIZE];			 	  // Видеокарта
	char net_int[2][7]; 			  	  // Сетевые интерфейсы
}hard_info;

struct graph_strings {
	char string_name[6][15];		  // Имена строк для граф. режимов
	float string_load[6];			  // Загруженность 
}graph_strings;

char* parsing(char *str, char *symbol, int count);
void get_number_from_str(char* str, int* value);

// ##### SYSTEM INFO #######################

void cpu_sys_info();
void mem_info();
void gpu_sys_info();
void get_sys_info();

// ##### HARD INFO #########################

void version_info();
void network_interaces();
void cpu_hard_info();
void get_hard_info();

// ##### OUTPUT ############################

void DieWithError(char *errorMessage);
void system_info_output();
void hardware_info_output();
void write_to_log();
void generate_html();
void full_output();
void ncurses_background();

// ##### NETWORK ###########################

void TCPWay(int port, int max_clnt, pthread_t* threadID);
void UDPWay(int port, int max_clnt, pthread_t* threadID);
