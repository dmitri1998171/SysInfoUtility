#include "protocol.h"   

int main(int argc, char *argv[]) {
	if(argc == 1)
		full_output();
	else {
		int res, optIdx, state = 0;
		static struct option long_opt[] = {
				{"hard_info", 0, 0, 'w'},
				{"sys_info", 0, 0, 's'},
				{"log", 0, 0, 'l'},
				{"html", 0, 0, 'h'},
				{"network", 1, 0, 'n'},
				{"help", 0, 0, '?'},
				{0,0,0,0}
		};

		while ((res = getopt_long(argc,argv,"wslhn", long_opt, &optIdx)) != -1) {
			switch(res) {
				// case '0': printf("without args\n"); break;
				case 'w': { get_hard_info(); out(); state = 1; break; }	// Вывод хар-к ПК
				case 's': { get_sys_info(); current_values_output(); state = 2; break; }	// Вывод текущ. знач.
				case 'l': { full_output(); write_to_log(); break; }	// Запись в лог
				case 'h': { full_output(); write_to_log(); generate_html(); break; }	// Запись в html
				case 'n': { 
					printf("protocol type(TCP/UDP): ");
					scanf("%s", type_proto);
					printf("port: ");
					scanf("%i", &port);
					printf("client count: ");
					scanf("%i", &client_count);
					
					pthread_mutex_init(&mutex, NULL);
					threadID = (pthread_t*) malloc(client_count * sizeof(pthread_t));

					TCPWay(port, type_proto, client_count, state); 
					break; }	// Отправка по интернет
				case '?': { printf("\n Usage: %s [OPTION]\n\n -w, --hard_info print hardware information\n -s, --sys_info  print system information\n -l, --log       output to \'sysInfo.log\' file\n -h, --html\t output to html\n -n, --network   send info to remote client\n -?, --help\t print this help\n\n By default, without options, the utility displays hardware and system information\n\n", argv[0]); break; }
			}
		}
	}

    return 0;
}