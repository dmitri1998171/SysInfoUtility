#include "../include/protocol.h"   

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

// ##### OUTPUT #############################

void DieWithError(char *errorMessage) {
    printf("\nERROR: %s\n\n", errorMessage);
    exit(1);
}

void system_info_output() {
	printf("CPU avg: %s\n", sys_info.cpuavg);
	printf("GPU: %i Mb\n", sys_info.gpuavg);
	printf("RAM: %i / %i Mb\n", mem.memAvail, mem.memTotal);
	printf("Swap: %i / %i Mb\n", mem.swapAvail, mem.swapTotal);
	printf("CPU temp: %i C\n\n", sys_info.cpu_temp_avg);
}

void hardware_info_output() {
    printf("Version: %s\n", hard_info.version);
    printf("Network interfaces: ");
	
	for(int i=0; i < hard_info.count; i++)	
		printf("%s ", hard_info.net_int[i]);
	
	printf("\nCPU: %s\n", hard_info.cpu);
	printf("CPU CORES: %i\n", hard_info.cpu_cores);
	printf("GPU: \n");
	printf("Resolution: %s", hard_info.resolution);
	printf("RAM: %i Mb\n", mem.memTotal);
	printf("Swap: %i Mb\n\n", mem.swapTotal);
}

void write_to_log() {
	if ((fp = fopen("sysInfo.log", "w")) != NULL) {
		fprintf(fp, "Version: %s\n", hard_info.version);
		fprintf(fp, "Network interfaces: ");
		
		for(int i = 0; i < hard_info.count; i++)	
			fprintf(fp, "%s ", hard_info.net_int[i]);

		fprintf(fp, "\nCPU:\t%s\n", hard_info.cpu);
		fprintf(fp, "CPU CORES:\t%i\n", hard_info.cpu_cores);
		fprintf(fp, "GPU: \n");
		fprintf(fp, "RAM: %i Mb\n", mem.memTotal);
		fprintf(fp, "Swap: %i Mb\n\n", mem.swapTotal);

		fprintf(fp, "CPU avg: %s\n", sys_info.cpuavg);
		fprintf(fp, "GPU:\n");
		fprintf(fp, "RAM: %i / %i Mb\n", mem.memAvail, mem.memTotal);
		fprintf(fp, "Swap: %i / %i Mb\n", mem.swapAvail, mem.swapTotal);
		fprintf(fp, "CPU temp: %i C\n\n", sys_info.cpu_temp_avg);
		fclose(fp);
	}
	else {
		DieWithError("Could not open sysInfo.log file");
	}
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
	fprintf(fdst, "\t\t<link rel=\"stylesheet\" href=\"include/style.css\"></link>\n");
	fprintf(fdst, "\t</head>\n");
    fprintf(fdst, "\t<body>\n");
	fprintf(fdst, "\t\t<div class=\"bg_block\">\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");
	fprintf(fdst, "\t\t\t\t<div class=\"subblock\" style=\"padding-bottom: 14px;\">\n");
	
	// Левая колнка - текст. инфа
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

	// Правая колонка - ползунки
	for(int i = 0; i < 6; i++) {
		if(i == 3)
        	fprintf(fdst, "<div class=\"subblock\"><br></div>\n");

		fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		fprintf(fdst, "\t\t\t\t<table  width=\"100%%\">\n");
		fprintf(fdst, "\t\t\t\t\t<tr>\n");

		// Смена цвета ползунков
		fprintf(fdst, "\t\t\t\t\t<td class=\"td_line\" width=\"80%%\"> ");
		if(graph_strings.string_load[i] > 50)
			fprintf(fdst, "<hr style=\" background-color: yellow;");
		else if(graph_strings.string_load[i] > 85)
			fprintf(fdst, "<hr style=\" background-color: red;");
		else fprintf(fdst, "<hr style=\"");
		fprintf(fdst, "width: %f%%;\"> </td>\n", graph_strings.string_load[i]);
		
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
	hardware_info_output();						// Вывод хар-к ПК
	printf("-------------------------------\n\n");
	system_info_output();	// Вывод текущих значений
}

void ncurses_sys_output(int line_pos) {
	mvprintw(line_pos,     1, "CPU avg: %s\n", sys_info.cpuavg);
	mvprintw(line_pos + 1, 1, "GPU: %i Mb\n", sys_info.gpuavg);
	mvprintw(line_pos + 2, 1, "RAM: %i / %i Mb\n", mem.memAvail, mem.memTotal);
	mvprintw(line_pos + 3, 1, "Swap: %i / %i Mb\n", mem.swapAvail, mem.swapTotal);
	mvprintw(line_pos + 4, 1, "CPU temp: %i C\n\n", sys_info.cpu_temp_avg);
}

void ncurses_hw_output(int line_pos) {
	mvprintw(line_pos, 1, "Version: %s\n", hard_info.version);
		
    mvprintw(line_pos + 1, 1, "Network interfaces:");
	// move(line_pos + 1, 20);
	// for(int i = 0; i < hard_info.count; i++) {
	// 	addch(' ');
	// 	for(int j = 0; j < strlen(hard_info.net_int[i]); j++) 
	// 		addch(hard_info.net_int[i][j]);
	// }
	
	mvprintw(line_pos + 2, 1, "CPU: %s\n", hard_info.cpu);
	mvprintw(line_pos + 3, 1, "CPU CORES: %i\n", hard_info.cpu_cores);
	mvprintw(line_pos + 4, 1, "GPU: \n");
	mvprintw(line_pos + 5, 1, "Resolution: %s", hard_info.resolution);
	mvprintw(line_pos + 6, 1, "RAM: %i Mb\n", mem.memTotal);
	mvprintw(line_pos + 7, 1, "Swap: %i Mb\n\n", mem.swapTotal);
}

void *ncurses_output() {
	get_hard_info();			// сбор хар-к ПК
	
	while(1) {
		get_sys_info();				// сбор текущих значений

		ncurses_hw_output(1);
		ncurses_sys_output(10);
		mvprintw(LINES - 1, 1, " ESC - Quit");
		refresh();
		sleep(1);
	}
	return (NULL);
}

void *ncurses_input(void *threadArgs) {
	pthread_t* output_tid = (pthread_t*) threadArgs;
	int value;

	// Если нажата клавиша ESCAPE - закрываем поток вывода ncurses_output
	while(value != KEY_F(1)) value = wgetch(stdscr);
	pthread_cancel(*output_tid);
	
	return (NULL);
}

void ncurses_background() {
	initscr();
    curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	
	pthread_mutex_init(&ncurses_mutex, NULL);
	pthread_t* threadID = (pthread_t*) malloc(2 * sizeof(pthread_t));
					
	if(pthread_create(&threadID[0], NULL, ncurses_output, NULL) != 0)
		DieWithError("pthread_create() failed");
	if(pthread_create(&threadID[1], NULL, ncurses_input, &threadID[0]) != 0)
		DieWithError("pthread_create() failed");
            
	if(pthread_join(threadID[0], NULL) != 0) 
		DieWithError("Joining the first ncurses_thread");
	if(pthread_join(threadID[1], NULL) != 0) 
		DieWithError("Joining the second ncurses_thread");

	pthread_mutex_destroy(&ncurses_mutex);
	endwin();
}
