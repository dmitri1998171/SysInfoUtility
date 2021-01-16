
#define N 50
#define ECHOMAX 20

int max_clnt, port;
char *type_proto;

struct mem {
	char memTotal[N];	//
	char memFree[N];	// ОЗУ
	char memAvail[N];	//
};

struct info {
	char net_int[N][N];	  // Сетевые интерфейсы
	int count;			  // счетчик кол-ва сетев. инетерфейсов
	char cpuavg[N];		  // Нагрузка процессора
	struct mem mem;		  // ОЗУ
};


