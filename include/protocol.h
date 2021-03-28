#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>

#define ARR_SIZE 50

FILE *fp;
pthread_mutex_t mutex, ncurses_mutex;
struct ThreadArgs *threadArgs;

struct ThreadArgs{
    int clntSock;
};

struct mem {
	int memTotal;	// Полный объем
	int memAvail;	// Используется
	int swapTotal;	// swap полный
	int swapAvail;	// swap исп.
}mem;

struct sys_info {
	float cpu_load;	  	  // Нагрузка процессора
	char cpuavg[15];	  // Нагрузка процессора
	int gpuavg;			  // Объем видеопамяти
	int cpu_temp_avg;	  // cpu temp
}sys_info;

struct hard_info {
    char version[ARR_SIZE];      	  // Версия ядра линукс
	char net_int[2][7]; 			  // Сетевые интерфейсы
	char cpu[ARR_SIZE];			 	  // Процессор
	int cpu_cores;		 	 		  // Кол-во ядер
	unsigned int count;	  			  // Кол-во сетев. инетерфейсов
	char resolution[ARR_SIZE/4];	  // Разрешение экрана
}hard_info;

struct graph_strings {
	char string_name[6][15];		  // Имена строк для граф. режимов
	float string_load[6];			  // Загруженность 
}graph_strings;

char* parsing(char *str, char *symbol);
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
